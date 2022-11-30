#include "FFDemux.h"
#include "BoLog.h"
extern "C" {
#include "libavformat/avformat.h"
}

namespace {
void printFFLog(int ret) {
    char errBuffer[1024]{0};
    av_strerror(ret, errBuffer, sizeof(errBuffer));
    BO_ERROR(errBuffer);
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
}

bool FFDemux::open(const char *url) {
    BO_INFO("open url begin", url);
    int ret = avformat_open_input(&ic, url, 0, 0);
    if (ret != 0) {
        printFFLog(ret);
        return false;
    }
    BO_INFO("open successfully", url);

    // 读取文件信息
    ret = avformat_find_stream_info(ic, 0);
    if (ret != 0) {
        printFFLog(ret);
        return false;
    }
    // duration时间计数单位，一秒计数1000000次，这个数值不一定有，
    // 因为可能信息在流里，不在封装的文件头中
    this->totalMs = ic->duration / AV_TIME_BASE * 1000;
    BO_INFO("total ms = {0}", totalMs);
    return true;
}

// 读取一帧数据
BoData FFDemux::read() {
    if (!ic) {
        return BoData();
    }
    BoData boData;
    AVPacket *pkt = av_packet_alloc();
    int ret = av_read_frame(ic, pkt);
    if (ret != 0) {
        av_packet_free(&pkt);
        return BoData();
    }
    boData.setData((unsigned char *)pkt);
    boData.setSize(pkt->size);

    BO_INFO("size {0}, pts {0}", pkt->size, pkt->pts);

    return boData;
}

int FFDemux::getTotalTime() { return totalMs; }
