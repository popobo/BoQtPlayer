#ifndef FFDEMUX_H
#define FFDEMUX_H

#include "IDemux.h"
extern "C" {
#include "libavformat/avformat.h"
}

class FFDemux : public IDemux {
  public:
    FFDemux();

    ~FFDemux();

    // IDemux interface
    virtual bool open(const char *url) override;
    virtual std::shared_ptr<IBoData> read() override;
    virtual int64_t getTotalTime() override;
    virtual bool seek(double pos) override;

    virtual std::shared_ptr<IParameter> getVideoParameter() override;
    virtual std::shared_ptr<IParameter> getAudioParameter() override;

    virtual void main() override;

  private:
    AVFormatContext *m_avFormatContext = nullptr;
    std::mutex m_avFormatContextMutex;
    
    std::shared_ptr<IParameter> m_videoParameter;
    std::shared_ptr<IParameter> m_audioParameter;

    int m_audioStream = 1;
    int m_videoStream = 0;

    int64_t m_totalMs = 0;
};

#endif // FFDEMUX_H
