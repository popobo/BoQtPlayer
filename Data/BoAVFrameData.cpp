#include "BoAVFrameData.h"
extern "C" {
#include "libavcodec/avcodec.h"
}

BoAVFrameData::BoAVFrameData() {}

BoAVFrameData::~BoAVFrameData() {}

BoAVFrameData::BoAVFrameData(const BoAVFrameData &dataIn) {
    AVFrame *newFrame = nullptr;
    av_frame_ref(newFrame, (AVFrame *)dataIn.structDataPtr());
    this->setStructDataPtr((void *)newFrame);
    this->setDatas(dataIn.datas());

    this->copyBasicAttributes(dataIn);
}

BoAVFrameData &BoAVFrameData::operator=(const BoAVFrameData &dataIn) {
    this->drop();

    AVFrame *newFrame = nullptr;
    av_frame_ref(newFrame, (AVFrame *)dataIn.structDataPtr());
    this->setStructDataPtr((void *)newFrame);
    this->setDatas(dataIn.datas());

    this->copyBasicAttributes(dataIn);

    return *this;
}

void BoAVFrameData::alloc() {}

void BoAVFrameData::drop() {
    if (!this->structDataPtr()) {
        return;
    }

    AVFrame *thisFrame = (AVFrame *)this->structDataPtr();
    av_frame_free(&thisFrame);

    this->setStructDataPtr(nullptr);
}
