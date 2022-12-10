#pragma once

#include "IBoData.h"

class BoAVPacketData : public IBoData {
  public:
    BoAVPacketData();

    virtual ~BoAVPacketData();

    BoAVPacketData(const BoAVPacketData &dataIn);

    BoAVPacketData &operator=(const BoAVPacketData &boData);

    virtual void alloc() override;

    virtual void drop() override;
};
