#ifndef BOPARAMETER_H
#define BOPARAMETER_H

#include <QAudioFormat>
#include <memory>
extern "C" {
#include "libavcodec/avcodec.h"
}

enum class SampleBits {
    Unknown = 0,
    UInt8,
    Int16,
    Int32,
    Float,
};

struct AudioOutputFormat {
    int sampleRate{0};
    int sampleChannelCount{0};
    SampleBits sampleBits{SampleBits::Unknown};
};

class BoParameter {
  public:
    BoParameter();

    ~BoParameter();

    BoParameter(const BoParameter &boParameter);

    BoParameter &operator=(const BoParameter &boParameter);

    AVCodecParameters *getPara() const;
    void setPara(AVCodecParameters *newPara);

    void setAudioOutputFormat(AudioOutputFormat newAudioOutFormat);

    AudioOutputFormat getAudioOutputFormat() const;

  private:
    AVCodecParameters *m_aVCodecParameters = nullptr;
    AudioOutputFormat m_outAudioFormat;
};

#endif // BOPARAMETER_H
