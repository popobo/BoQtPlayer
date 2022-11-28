#include "BoData.h"
extern "C" {
#include "libavformat/avformat.h"
}

void BoData::drop() {
    if (!data) {
        return;
    }
    av_packet_free((AVPacket **)&data);
    data = 0;
    size = 0;
}
