#pragma once
#include "IParameter.h"
#include "IObserver.h"
#include "ISubject.h"

class IResampler : public IObserver, public ISubject {
  public:
    IResampler();

    ~IResampler();

    virtual bool open(const std::shared_ptr<IParameter> &parameterIn,
                      const std::shared_ptr<IParameter> &parameterOut) = 0;

    virtual bool open(const std::shared_ptr<IParameter> &parameterIn,
                      const AudioOutputFormat &parameterOut) = 0;

    virtual std::shared_ptr<IBoData>
    resample(const std::shared_ptr<IBoData> &boDataIn) = 0;

    virtual void update(std::shared_ptr<IBoData> boData) override;

  private:
    int outChannels = 2;
    int outFormat = 1;
};

