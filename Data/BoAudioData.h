#ifndef BOAUDIODATA_H
#define BOAUDIODATA_H

#include "IBoData.h"

class BoAudioData : public IBoData {
  public:
    BoAudioData();

    virtual ~BoAudioData();

    BoAudioData(const BoAudioData &dataIn);

    BoAudioData &operator=(const BoAudioData &boData);

    virtual bool alloc(int size, const uint8_t *dataIn) override;
    virtual void drop() override;
};

#endif // BOAUDIODATA_H
