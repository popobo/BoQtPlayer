#ifndef FFParameter_H
#define FFParameter_H

#include <IParameter.h>
#include <memory>

class FFParameter : public IParameter {
  public:
    FFParameter();

    ~FFParameter();

    FFParameter(const FFParameter &FFParameter);

    FFParameter &operator=(const FFParameter &FFParameter);

    virtual void *getPara() const override;
    virtual void setPara(void *newPara) override;

    virtual void
    setAudioOutputFormat(AudioOutputFormat newAudioOutFormat) override;

    virtual AudioOutputFormat getAudioOutputFormat() const override;

  private:
    void *m_aVCodecParameters = nullptr;
    AudioOutputFormat m_outAudioFormat;
};

#endif // FFParameter_H
