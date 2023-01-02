#include "FFDemux.h"
#include "BoLog.h"
#include "BoUtil.h"
#include "Data/BoAVPacketData.h"
#include "FFParameter.h"
extern "C" {
#include "libavformat/avformat.h"
}

namespace {
double r2d(AVRational ration) {
    return ration.num == 0 || ration.den == 0
               ? 0.0
               : (double)ration.num / (double)ration.den;
}
} // namespace

FFDemux::FFDemux() {
    // this is not thraed safe, do not create these together
    // need a check, it seems that this is safe in C++11
    static bool isFirst = true;
    if (isFirst) {
        isFirst = false;
        // av_register_all(); no need,
        // https://cloud.tencent.com/developer/article/1910867
        // avcodec_register_all(); no need,
        // https://blog.csdn.net/u011686167/article/details/121723790
        avformat_network_init();
        BO_INFO("ffmepg init");
    }
    m_videoParameter = std::make_shared<FFParameter>();
    m_audioParameter = std::make_shared<FFParameter>();
}

FFDemux::~FFDemux() {
    m_avFormatContextMutex.lock();
    avformat_close_input(&m_avFormatContext);
    m_avFormatContextMutex.unlock();
}

bool FFDemux::open(const char *url) {
    BO_INFO("open url begin", url);

    std::unique_lock<std::mutex> locker{m_avFormatContextMutex};

    int ret = avformat_open_input(&m_avFormatContext, url, 0, 0);
    if (ret != 0) {
        PRINT_FFMPEG_ERROR(ret);
        return false;
    }
    BO_INFO("open successfully", url);

    // 读取文件信息
    ret = avformat_find_stream_info(m_avFormatContext, 0);
    if (ret != 0) {
        PRINT_FFMPEG_ERROR(ret);
        return false;
    }
    // duration时间计数单位，一秒计数1000000次，这个数值不一定有，
    // 因为可能信息在流里，不在封装的文件头中
    m_totalMs = m_avFormatContext->duration / AV_TIME_BASE * 1000;
    BO_INFO("total ms = {0}", m_totalMs);

    ret = av_find_best_stream(m_avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1,
                              nullptr, 0);
    if (ret < 0) {
        BO_ERROR("av_find_best_stream failed");
    }
    m_videoStream = ret;
    m_videoParameter->setPara(m_avFormatContext->streams[ret]->codecpar);
    auto videoTimeBase = m_avFormatContext->streams[ret]->time_base;
    auto doubleVideoTimeBase = videoTimeBase.den == 0
                                   ? 0.0
                                   : static_cast<double>(videoTimeBase.num) /
                                         static_cast<double>(videoTimeBase.den);

    //获取音频流索引
    ret = av_find_best_stream(m_avFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1,
                              nullptr, 0);
    if (ret < 0) {
        BO_ERROR("av_find_best_stream failed");
    }
    m_audioStream = ret;
    m_audioParameter->setPara(m_avFormatContext->streams[ret]->codecpar);
    auto audioTimeBase = m_avFormatContext->streams[ret]->time_base;
    auto doubleAudioTimeBase = audioTimeBase.den == 0
                                   ? 0.0
                                   : static_cast<double>(audioTimeBase.num) /
                                         static_cast<double>(audioTimeBase.den);

    m_audioParameter->setAudioTimeBase(doubleAudioTimeBase * 1000);
    m_audioParameter->setVideoTimeBase(doubleVideoTimeBase * 1000);

    m_videoParameter->setAudioTimeBase(doubleAudioTimeBase * 1000);
    m_videoParameter->setVideoTimeBase(doubleVideoTimeBase * 1000);

    return true;
}

// 读取一帧数据
std::shared_ptr<IBoData> FFDemux::read() {
    auto boData = std::make_shared<BoAVPacketData>();

    std::unique_lock<std::mutex> locker{m_avFormatContextMutex};

    if (!m_avFormatContext) {
        return boData;
    }

    AVPacket *pkt = av_packet_alloc();
    int ret = av_read_frame(m_avFormatContext, pkt);
    if (ret != 0) {
        av_packet_free(&pkt);
        return boData;
    }

    boData->setStructDataPtr((void *)pkt);
    boData->setSize(pkt->size);

    if (pkt->stream_index == m_audioStream) {
        boData->setIsAudio(true);
    } else if (pkt->stream_index == m_videoStream) {
        boData->setIsAudio(false);
    } else {
        av_packet_free(&pkt);
        return boData;
    }

    // 转换pts(ms)
    pkt->pts = (int64_t)(pkt->pts *
                         (r2d(m_avFormatContext->streams[pkt->stream_index]
                                  ->time_base)) *
                         1000);
    pkt->dts = (int64_t)(pkt->dts *
                         (r2d(m_avFormatContext->streams[pkt->stream_index]
                                  ->time_base)) *
                         1000);
    boData->setPts(pkt->pts);

    return boData;
}

int64_t FFDemux::getTotalTime() { return m_totalMs; }

std::shared_ptr<IParameter> FFDemux::getVideoParameter() {
    return m_videoParameter;
}

std::shared_ptr<IParameter> FFDemux::getAudioParameter() {
    return m_audioParameter;
}

void FFDemux::main() {
    if (isAnyObserverSatisfied()) {
        return;
    }

    auto boData = read();
    if (boData->size() > 0) {
        //通知观察者, 如果没有观察者接受数据, 数据应该销毁
        notify(boData);
    }
}

bool FFDemux::seek(double pos) {
    if (pos < 0 || pos > 1) {
        BO_ERROR("seek value must 0.0-1.0");
        return false;
    }

    std::unique_lock<std::mutex> locker{m_avFormatContextMutex};

    bool re = false;
    if (!m_avFormatContext) {
        return false;
    }

    //清空读取的缓冲, read只取一帧, 缓冲中可能还有帧
    avformat_flush(m_avFormatContext);
    auto seekPts = static_cast<int64_t>(
        m_avFormatContext->streams[m_videoStream]->duration * pos);
    //往后(进度条左边)跳转到关键帧
    re = av_seek_frame(m_avFormatContext, m_videoStream, seekPts,
                       AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);

    return re;
}
