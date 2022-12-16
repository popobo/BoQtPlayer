#ifndef IPARAMETER_H
#define IPARAMETER_H

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

class IParameter {
  public:
    IParameter();

    ~IParameter() {}

    virtual void *getPara() const = 0;
    virtual void setPara(void *newPara) = 0;

    virtual void setAudioOutputFormat(AudioOutputFormat newAudioOutFormat) = 0;

    virtual AudioOutputFormat getAudioOutputFormat() const = 0;

  private:
    void *m_aVCodecParameters = nullptr;
    AudioOutputFormat m_outAudioFormat;
};

#endif // IPARAMETER_H
