#pragma once

#include "IBoData.h"

class BoAVPacketData : public IBoData {
  public:
    BoAVPacketData();

    virtual ~BoAVPacketData();

    BoAVPacketData(const BoAVPacketData &dataIn);

    BoAVPacketData &operator=(const BoAVPacketData &boData);

    virtual void drop() override;

    virtual void setStructDataPtr(void *newStructDataPtr) override;
};
