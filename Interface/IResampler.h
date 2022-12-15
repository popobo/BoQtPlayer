#ifndef IRESAMPLER_H
#define IRESAMPLER_H

#include "FFParameter.h"
#include "IObserver.h"
#include "ISubject.h"

class IResampler : public IObserver, public ISubject {
  public:
    IResampler();

    ~IResampler();

    virtual bool open(const FFParameter &parameterIn,
                      const FFParameter &parameterOut) = 0;

    virtual bool open(const FFParameter &parameterIn,
                      const AudioOutputFormat &parameterOut) = 0;

    virtual std::shared_ptr<IBoData>
    resample(const std::shared_ptr<IBoData> &boDataIn) = 0;

    virtual void update(const std::shared_ptr<IBoData> &boData) override;

  private:
    int outChannels = 2;
    int outFormat = 1;
};

#endif // IRESAMPLER_H
