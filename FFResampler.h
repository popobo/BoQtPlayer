#ifndef FFRESAMPLER_H
#define FFRESAMPLER_H

#include "IResampler.h"
#include <mutex>
extern "C" {
#include "libswresample/swresample.h"
}

class FFResampler : public IResampler {
  public:
    FFResampler();

    ~FFResampler();

    virtual bool open(const BoParameter &parameterIn,
                      const BoParameter &parameterOut) override;

    virtual bool open(const BoParameter &parameterIn,
                      const AudioOutputFormat &audioOutputFormat) override;

    virtual std::shared_ptr<IBoData>
    resample(const std::shared_ptr<IBoData> &boDataIn) override;

  protected:
    SwrContext *m_swrContext{nullptr};
    AVChannelLayout m_channelLayout;
    AVSampleFormat m_sampleFormat;
    std::mutex m_swrContextMutex;

    int outChannels{0};
    int outFormat{0};
};

#endif // FFRESAMPLER_H
