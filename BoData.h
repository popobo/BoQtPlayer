#ifndef BODATA_H
#define BODATA_H
extern "C" {
#include "libavformat/avformat.h"
}

struct BoData {
    unsigned char *data{nullptr};
    unsigned char *datas[AV_NUM_DATA_POINTERS]{nullptr};
    int size{0};
    int width{0};
    int height{0};
    int format{0};
    bool isAudio{false};

    void drop();
};

#endif // BODATA_H
