#include "FFDecoder.h"
#include "BoLog.h"
#include "BoUtil.h"
#include "Data/BoAVFrameData.h"
extern "C" {
#include "libswscale/swscale.h"
}
#include <QImage>

FFDecoder::FFDecoder() {}

FFDecoder::~FFDecoder() { BO_ERROR(""); }

bool FFDecoder::open(const std::shared_ptr<IParameter> &parameter) {
    if (!parameter->getPara()) {
        return false;
    }

    auto para = (AVCodecParameters *)parameter->getPara();

    // 1.查找解码器
    const AVCodec *avc = avcodec_find_decoder(para->codec_id);
    if (!avc) {
        BO_INFO("avcodec_find_decoder %d failed!", (int)para->codec_id);
    }
    BO_INFO("avcodec_find_decoder {0} successfully!", (int)para->codec_id);

    std::unique_lock<std::mutex> locker{m_codecContextMutex};
    // 2.创建解码器上下文，并复制参数
    m_codecContext = avcodec_alloc_context3(avc);
    avcodec_parameters_to_context(m_codecContext, para);

    //多线程解码
    m_codecContext->thread_count = 8;
    // 3.打开解码器
    int ret = avcodec_open2(m_codecContext, nullptr, nullptr);
    if (ret != 0) {
        PRINT_FFMPEG_ERROR(ret);
        return false;
    }
    BO_INFO("avcodec_open2 successfully");

    if (AVMEDIA_TYPE_VIDEO == m_codecContext->codec_type) {
        m_isAudio = false;
        m_videoTimeBase = parameter->videoTimeBase();
    } else if (AVMEDIA_TYPE_AUDIO == m_codecContext->codec_type) {
        m_isAudio = true;
        m_audioTimeBase = parameter->audioTimeBase();
    }

    return true;
}

bool FFDecoder::sendPacket(const std::shared_ptr<IBoData> &boData) {
    if (boData->size() <= 0 || !boData->structDataPtr()) {
        return false;
    }

    std::unique_lock<std::mutex> locker{m_codecContextMutex};
    if (!m_codecContext) {
        return false;
    }

    int ret = avcodec_send_packet(m_codecContext,
                                  (AVPacket *)boData->structDataPtr());
    if (ret != 0) {
        PRINT_FFMPEG_ERROR(ret);
        return false;
    }
    return true;
}

std::shared_ptr<IBoData> FFDecoder::recvFrame() {
    auto boData = std::make_shared<BoAVFrameData>();

    std::unique_lock<std::mutex> codecContextlocker{m_codecContextMutex};

    if (!m_codecContext) {
        return nullptr;
    }

    std::unique_lock<std::mutex> frameLocker{m_frameMutex};
    if (!m_frame) {
        m_frame = av_frame_alloc();
    }

    //再次调用会复用上次空间，线程不安全
    int ret = avcodec_receive_frame(m_codecContext, m_frame);
    if (ret != 0) {
        // 读到空了
        return nullptr;
    }
    // 这边对m_frame底下的AVBuffer产生了两次引用，
    // 一次是m_frame自身，一个是在boData里
    boData->setStructDataPtr((void *)m_frame);

    if (AVMEDIA_TYPE_VIDEO == m_codecContext->codec_type) {
        // yuv
        boData->setSize((m_frame->linesize[0] + m_frame->linesize[1] +
                         m_frame->linesize[2]) *
                        m_frame->height);

        boData->setWidth(m_frame->width);
        boData->setHeight(m_frame->height);
        boData->setIsAudio(false);
        boData->setTimeBase(m_videoTimeBase);

    } else if (AVMEDIA_TYPE_AUDIO == m_codecContext->codec_type) {
        //样本字节数 * 单通道样本数 * 通道数
        boData->setSize(
            av_get_bytes_per_sample((AVSampleFormat)m_frame->format) *
            m_frame->nb_samples * m_frame->ch_layout.nb_channels);
        boData->setIsAudio(true);
        boData->setTimeBase(m_audioTimeBase);
    }

    codecContextlocker.unlock();

    // m_frame->data什么时候回收,
    // FFmpeg通过引用计数机制保证，使用需要保证相关方法调用正确
    for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
        boData->addDatas(m_frame->data[i]);
    }

    // BO_INFO("m_isAudio: {0}, m_frame->pts: {1}", m_isAudio, m_frame->pts);
    boData->setPts(static_cast<int>(m_frame->pts));

    return boData;
}

void FFDecoder::close() {
    m_boDataQueueMutex.lock();
    m_boDataQueue = {};
    m_boDataQueueMutex.unlock();

    m_codecContextMutex.lock();
    if (m_codecContext) {
        avcodec_flush_buffers(m_codecContext);
        avcodec_close(m_codecContext);
        avcodec_free_context(&m_codecContext);
    }
    m_codecContextMutex.unlock();

    m_frameMutex.lock();
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    m_frameMutex.unlock();
}

void FFDecoder::clear() {
    m_boDataQueueMutex.lock();
    m_boDataQueue = {};
    m_boDataQueueMutex.unlock();

    m_codecContextMutex.lock();
    if (m_codecContext) {
        avcodec_flush_buffers(m_codecContext);
    }
    m_codecContextMutex.unlock();
}

bool FFDecoder::isAudio() const { return m_isAudio; }

void FFDecoder::setIsAudio(bool newIsAudio) { m_isAudio = newIsAudio; }

void FFDecoder::main() {
    std::unique_lock<std::mutex> lock(m_boDataQueueMutex);
    //取出packet 消费者
    if (m_boDataQueue.empty()) {
        m_isStatified = false;
        m_isDecodedDataLeftLastTime = false;
        lock.unlock();
        boSleep(1);
        return;
    }

    if (m_isDecodedDataLeftLastTime) {
        while (true) {
            if (isAnyObserverSatisfied()) {
                m_isDecodedDataLeftLastTime = true;
                break;
            }

            // 获取上次未读取完的解码数据
            auto frame = recvFrame();
            if (!frame) {
                m_isDecodedDataLeftLastTime = false;
                break;
            }

            notify(frame);
        }
        return;
    }

    std::shared_ptr<IBoData> boData = m_boDataQueue.front();
    m_boDataQueue.pop();

    if (m_boDataQueue.size() < 0.5 * MAX_LIST) {
        m_isStatified = false;
    }

    //开启解码
    //发送数据到解码线程 一个数据包可能解码多个结果(主要是音频)
    if (sendPacket(boData)) {
        while (true) {
            if (isAnyObserverSatisfied()) {
                m_isDecodedDataLeftLastTime = true;
                break;
            }

            //获取解码器
            //获取解码数据
            auto frame = recvFrame();
            if (!frame) {
                break;
            }

            //发送数据给观察者
            notify(frame);
        }
    }

    //消费者负责清理, 采用智能指针管理
}

bool FFDecoder::isSatisfied() { return m_isStatified; }

void FFDecoder::update(const std::shared_ptr<IBoData> &boData) {
    if (boData->isAudio() != m_isAudio) {
        return;
    }

    // 循环是为了阻塞住FFDemux让其不要读取数据了
    if (m_boDataQueue.size() > 0.75 * MAX_LIST) {
        // 观察者已经满足了
        m_isStatified = true;
    }

    std::unique_lock<std::mutex> lock(m_boDataQueueMutex);
    if (m_boDataQueue.size() < MAX_LIST) {
        m_boDataQueue.push(boData);
    }
}
