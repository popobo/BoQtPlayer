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

    void *getPara() const;
    void setPara(void *newPara);

    void setAudioOutputFormat(AudioOutputFormat newAudioOutFormat);

    AudioOutputFormat getAudioOutputFormat() const;

  private:
    void *m_aVCodecParameters = nullptr;
    AudioOutputFormat m_outAudioFormat;
};

#endif // FFParameter_H
