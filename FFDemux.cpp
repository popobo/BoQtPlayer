#include "FFDemux.h"
#include "BoLog.h"
#include "BoUtil.h"
#include "Data/BoAVPacketData.h"
extern "C" {
#include "libavformat/avformat.h"
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
    this->totalMs = ic->duration / AV_TIME_BASE * 1000;
    BO_INFO("total ms = {0}", totalMs);

    getVideoParameter();
    getAudioParameter();

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

    return boData;
}

int64_t FFDemux::getTotalTime() { return totalMs; }

BoParameter FFDemux::getVideoParameter() {
    if (!ic) {
        BO_ERROR("ic is nullptr");
        return BoParameter();
    }
    //获取视频流索引
    int ret = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (ret < 0) {
        BO_ERROR("av_find_best_stream failed");
        return BoParameter();
    }
    m_videoStream = ret;
    BoParameter parameter;
    parameter.setPara(ic->streams[ret]->codecpar);
    return parameter;
}

BoParameter FFDemux::getAudioParameter() {
    if (!ic) {
        BO_ERROR("ic is nullptr");
        return BoParameter();
    }
    //获取视频流索引
    int ret = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (ret < 0) {
        BO_ERROR("av_find_best_stream failed");
        return BoParameter();
    }
    m_audioStream = ret;
    BoParameter parameter;
    parameter.setPara(ic->streams[ret]->codecpar);
    return parameter;
}
