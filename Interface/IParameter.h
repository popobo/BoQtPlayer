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

    double timeBase() const;
    void setTimeBase(double newTimeBase);

  private:
    void *m_aVCodecParameters = nullptr;
    AudioOutputFormat m_outAudioFormat;
    // 单位 ms
    double m_timeBase = 0.0;
};

#endif // IPARAMETER_H
