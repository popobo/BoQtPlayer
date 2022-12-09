#include "BoData.h"
#include "BoLog.h"
extern "C" {
#include "libavcodec/avcodec.h"
}

BoData::BoData() {}

BoData::~BoData() { drop(); }

void BoData::drop() {
    if (!structData) {
        return;
    }
    // 应该根据不同类型调用不同释放函数
    switch (this->dataType) {
    // case不加{}的话，不能初始化变量
    case DataType::FF_AVPacket: {
        // 释放原来的AVPacket
        AVPacket *thisAVPacket = (AVPacket *)this->structData;
        av_packet_free(&thisAVPacket);
        break;
    }
    case DataType::FF_AVFrame: {
        //释放原来的AVFrame
        AVFrame *thisFrame = (AVFrame *)this->structData;
        av_frame_free(&thisFrame);
        break;
    }
    default:
        break;
    }
    structData = nullptr;
}

BoData::BoData(const BoData &boData) {
    switch (boData.dataType) {
    // case不加{}的话，不能初始化变量
    case DataType::FF_AVPacket: {
        // 浅拷贝，只增加AVBuffer的引用计数
        AVPacket *newPkt = nullptr;
        av_packet_ref(newPkt, (AVPacket *)boData.structData);
        this->structData = (unsigned char *)newPkt;
        break;
    }
    case DataType::FF_AVFrame: {
        // 浅拷贝，只增加AVBuffer的引用计数
        AVFrame *newFrame = nullptr;
        av_frame_ref(newFrame, (AVFrame *)boData.structData);
        this->structData = (unsigned char *)newFrame;
        memcpy(this->datas, boData.datas, sizeof(this->datas));
        break;
    }
    default:
        break;
    }

    this->size = boData.size;
    this->width = boData.width;
    this->height = boData.height;
    this->format = boData.format;
    this->isAudio = boData.isAudio;
    this->dataType = boData.dataType;
}

BoData &BoData::operator=(const BoData &boData) {
    this->drop();

    switch (boData.dataType) {
    // case不加{}的话，不能初始化变量
    case DataType::FF_AVPacket: {
        // 浅拷贝，只增加AVBuffer的引用计数
        AVPacket *newPkt = nullptr;
        av_packet_ref(newPkt, (AVPacket *)boData.structData);
        this->structData = (unsigned char *)newPkt;
        break;
    }
    case DataType::FF_AVFrame: {
        // 浅拷贝，只增加AVBuffer的引用计数
        AVFrame *newFrame = nullptr;
        av_frame_ref(newFrame, (AVFrame *)boData.structData);
        this->structData = (unsigned char *)newFrame;
        memcpy(this->datas, boData.datas, sizeof(this->datas));
        break;
    }
    default:
        break;
    }

    this->size = boData.size;
    this->width = boData.width;
    this->height = boData.height;
    this->format = boData.format;
    this->isAudio = boData.isAudio;
    this->dataType = boData.dataType;
    return *this;
}
