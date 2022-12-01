#include "BoData.h"
#include "BoLog.h"

void BoData::drop() {
    if (!data) {
        return;
    }
    av_packet_free((AVPacket **)&data);
    data = nullptr;
    size = 0;
}
