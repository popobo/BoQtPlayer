#include "FFDecoder.h"
#include "BoLog.h"
#include "BoUtil.h"
#include "Data/BoAVFrameData.h"
extern "C" {
#include "libswscale/swscale.h"
}
#include <QImage>

FFDecoder::FFDecoder() {}

FFDecoder::~FFDecoder() {}

bool FFDecoder::open(const BoParameter &parameter) {
    if (!parameter.getPara()) {
        return false;
    }
    auto para = parameter.getPara();
    // 1.查找解码器
    const AVCodec *avc = avcodec_find_decoder(para->codec_id);
    if (!avc) {
        BO_INFO("avcodec_find_decoder %d failed!", para->codec_id);
    }
    BO_INFO("avcodec_find_decoder {0} successfully!", para->codec_id);
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
    } else if (AVMEDIA_TYPE_AUDIO == m_codecContext->codec_type) {
        m_isAudio = true;
    }

    return true;
}

bool FFDecoder::sendPacket(const std::shared_ptr<IBoData> &boData) {
    if (boData->size() <= 0 || !boData->structDataPtr()) {
        return false;
    }

    if (!m_codecContext) {
        return false;
    }

    int ret =
        avcodec_send_packet(m_codecContext, (AVPacket *)boData->structDataPtr());
    if (ret != 0) {
        PRINT_FFMPEG_ERROR(ret);
        return false;
    }
    return true;
}

std::shared_ptr<IBoData> FFDecoder::recvFrame() {
    auto boData = std::make_shared<BoAVFrameData>();
    if (!m_codecContext) {
        return nullptr;
    }

    if (!m_frame) {
        m_frame = av_frame_alloc();
    }

    //再次调用会复用上次空间，线程不安全
    int ret = avcodec_receive_frame(m_codecContext, m_frame);
    if (ret != 0) {
        // sPRINT_FFMPEG_ERROR(ret);
        return nullptr;
    }

    boData->setStructDataPtr((void *)m_frame);

    if (AVMEDIA_TYPE_VIDEO == m_codecContext->codec_type) {
        // yuv
        boData->setSize((m_frame->linesize[0] + m_frame->linesize[1] +
                        m_frame->linesize[2]) *
                       m_frame->height);

        boData->setWidth(m_frame->width);
        boData->setHeight(m_frame->height);

    } else if (AVMEDIA_TYPE_AUDIO == m_codecContext->codec_type) {
        //样本字节数 * 单通道样本数 * 通道数
        boData->setSize(
            av_get_bytes_per_sample((AVSampleFormat)m_frame->format) *
            m_frame->nb_samples * m_frame->ch_layout.nb_channels);
    }
    // m_frame->data什么时候回收, FFmpeg通过引用计数机制保证，使用需要保证相关方法调用正确
    for(int i = 0; i < AV_NUM_DATA_POINTERS; ++i){
        boData->addDatas(m_frame->data[i]);
    }

    return boData;
}

void FFDecoder::close() {}
