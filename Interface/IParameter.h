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

    void *getPara() const;
    void setPara(void *newPara);

    void setAudioOutputFormat(AudioOutputFormat newAudioOutFormat);

    AudioOutputFormat getAudioOutputFormat() const;

  private:
    void *m_aVCodecParameters = nullptr;
    AudioOutputFormat m_outAudioFormat;
};

#endif // IPARAMETER_H
