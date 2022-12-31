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
        return ration.num == 0 || ration.den == 0 ? 0.0 : (double)ration.num / (double)ration.den;
    }
}

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
    m_thread = std::make_shared<BoThread>();
}

FFDemux::~FFDemux() { avformat_close_input(&ic); }

bool FFDemux::open(const char *url) {
    BO_INFO("open url begin", url);
    int ret = avformat_open_input(&ic, url, 0, 0);
    if (ret != 0) {
        PRINT_FFMPEG_ERROR(ret);
        return false;
    }
    BO_INFO("open successfully", url);

    // 读取文件信息
    ret = avformat_find_stream_info(ic, 0);
    if (ret != 0) {
        PRINT_FFMPEG_ERROR(ret);
        return false;
    }
    // duration时间计数单位，一秒计数1000000次，这个数值不一定有，
    // 因为可能信息在流里，不在封装的文件头中
    m_totalMs = ic->duration / AV_TIME_BASE * 1000;
    BO_INFO("total ms = {0}", m_totalMs);

    ret = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (ret < 0) {
        BO_ERROR("av_find_best_stream failed");
    }
    m_videoStream = ret;
    m_videoParameter->setPara(ic->streams[ret]->codecpar);
    auto videoTimeBase = ic->streams[ret]->time_base;
    auto doubleVideoTimeBase = videoTimeBase.den == 0
                                   ? 0.0
                                   : static_cast<double>(videoTimeBase.num) /
                                         static_cast<double>(videoTimeBase.den);

    //获取音频流索引
    ret = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (ret < 0) {
        BO_ERROR("av_find_best_stream failed");
    }
    m_audioStream = ret;
    m_audioParameter->setPara(ic->streams[ret]->codecpar);
    auto audioTimeBase = ic->streams[ret]->time_base;
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
    if (!ic) {
        return boData;
    }

    AVPacket *pkt = av_packet_alloc();
    int ret = av_read_frame(ic, pkt);
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
    pkt->pts = (int64_t)(pkt->pts * (r2d(ic->streams[pkt->stream_index]->time_base)) * 1000);
    pkt->dts = (int64_t)(pkt->dts * (r2d(ic->streams[pkt->stream_index]->time_base)) * 1000);
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

bool FFDemux::start()
{
    bool ret = m_thread->start();
    std::weak_ptr<FFDemux> wself = shared_from_this();
    m_thread->addMainTask([wself]() {
        if (auto self = wself.lock()) {
            self->main();
        }
        });
    return ret;
}

void FFDemux::stop()
{
    m_thread->stop();
}

bool FFDemux::isPaused()
{
    return m_thread->isPaused();
}

void FFDemux::pause()
{
    m_thread->pause();
}

void FFDemux::resume()
{
    m_thread->resume();
}

void FFDemux::main() {
    auto boData = read();
    if (boData->size() > 0) {
        //通知观察者, 如果没有观察者接受数据, 数据应该销毁
        notify(boData);
    }
    else {
        boSleep(1);
    }
}


bool FFDemux::seek(double pos)
{
    return true;
}