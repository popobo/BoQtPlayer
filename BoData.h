#pragma once

extern "C" {
#include "libavformat/avformat.h"
}

enum class DataTyep {

};

struct BoData {
    unsigned char *data{nullptr};
    unsigned char *datas[AV_NUM_DATA_POINTERS]{nullptr};
    int size{0};
    int width{0};
    int height{0};
    int format{0};
    bool isAudio{false};

    void drop();

    BoData(const BoData &data);
};
