#ifndef BOAVFRAMEDATA_H
#define BOAVFRAMEDATA_H

#include "IBoData.h"

class BoAVFrameData : public IBoData {
  public:
    BoAVFrameData();

    virtual ~BoAVFrameData();

    BoAVFrameData(const BoAVFrameData &dataIn);

    BoAVFrameData &operator=(const BoAVFrameData &boData);

    virtual void drop() override;

    virtual void setStructDataPtr(void *newStructDataPtr) override;
};

#endif // BOAVFRAMEDATA_H
