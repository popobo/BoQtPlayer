#include "BoAVFrameData.h"
extern "C" {
#include "libavcodec/avcodec.h"
}

BoAVFrameData::BoAVFrameData() {}

BoAVFrameData::~BoAVFrameData() {
    if (!this->structDataPtr()) {
        return;
    }

    AVFrame *thisFrame = (AVFrame *)this->structDataPtr();
    av_frame_free(&thisFrame);
}

BoAVFrameData::BoAVFrameData(const BoAVFrameData &dataIn) {
    AVFrame *newFrame = av_frame_alloc();
    av_frame_ref(newFrame, (AVFrame *)dataIn.structDataPtr());
    this->m_structDataPtr = (void *)newFrame;
    this->setDatas(dataIn.datas());

    this->copyBasicAttributes(dataIn);
}

BoAVFrameData &BoAVFrameData::operator=(const BoAVFrameData &dataIn) {
    this->drop();

    AVFrame *newFrame = av_frame_alloc();
    av_frame_ref(newFrame, (AVFrame *)dataIn.structDataPtr());
    this->m_structDataPtr = (void *)newFrame;
    this->setDatas(dataIn.datas());

    this->copyBasicAttributes(dataIn);

    return *this;
}

void BoAVFrameData::drop() {
    if (!this->structDataPtr()) {
        return;
    }

    AVFrame *thisFrame = (AVFrame *)this->structDataPtr();
    av_frame_free(&thisFrame);

    this->setStructDataPtr(nullptr);
}

void BoAVFrameData::setStructDataPtr(void *newStructDataPtr) {
    //必须要复制一份, 否则会导致AVBuffer计数出问题
    AVFrame *newFrame = av_frame_alloc();
    av_frame_ref(newFrame, (AVFrame *)newStructDataPtr);
    m_structDataPtr = (void *)newFrame;
}
