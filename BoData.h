#pragma once

extern "C" {
#include "libavformat/avformat.h"
}

enum class DataType { NONE, FF_AVPacket, FF_AVFrame };

struct BoData {
    BoData();

    ~BoData();

    BoData(const BoData &boData);

    BoData &operator=(const BoData &boData);

    // 存储(char*)AVPacket, (char*)AVFrame等结构体数据
    unsigned char *structData{nullptr};
    // 存储AVFrame中的帧数据
    unsigned char *datas[AV_NUM_DATA_POINTERS]{nullptr};
    int size{0};
    int width{0};
    int height{0};
    int format{0};
    bool isAudio{false};
    DataType dataType{DataType::NONE};

    void drop();
};
