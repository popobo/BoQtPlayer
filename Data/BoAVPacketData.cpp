#include "BoAVPacketData.h"
extern "C" {
#include "libavcodec/avcodec.h"
}

BoAVPacketData::BoAVPacketData() {}

BoAVPacketData::~BoAVPacketData() {
    //析构函数里不能调用虚函数
    if (!this->structDataPtr()) {
        return;
    }

    // 释放原来的AVPacket
    AVPacket *thisAVPacket = (AVPacket *)this->structDataPtr();
    av_packet_free(&thisAVPacket);

    this->setStructDataPtr(nullptr);
}

BoAVPacketData::BoAVPacketData(const BoAVPacketData &dataIn) {
    // 浅拷贝，只增加AVBuffer的引用计数
    AVPacket *newPkt = nullptr;
    av_packet_ref(newPkt, (AVPacket *)dataIn.structDataPtr());
    this->setStructDataPtr((void *)newPkt);

    this->copyBasicAttributes(dataIn);
}

BoAVPacketData &BoAVPacketData::operator=(const BoAVPacketData &dataIn) {
    this->drop();
    AVPacket *newPkt = nullptr;
    av_packet_ref(newPkt, (AVPacket *)dataIn.structDataPtr());
    this->setStructDataPtr((void *)newPkt);
    this->copyBasicAttributes(dataIn);

    return *this;
}

void BoAVPacketData::alloc() {}

void BoAVPacketData::drop() {
    if (!this->structDataPtr()) {
        return;
    }

    // 释放原来的AVPacket
    AVPacket *thisAVPacket = (AVPacket *)this->structDataPtr();
    av_packet_free(&thisAVPacket);

    this->setStructDataPtr(nullptr);
}
