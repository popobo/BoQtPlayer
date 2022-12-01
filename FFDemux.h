#ifndef FFDEMUX_H
#define FFDEMUX_H

#include "IDemux.h"
extern "C" {
#include "libavformat/avformat.h"
}

class FFDemux : public IDemux {
  public:
    FFDemux();

    // IDemux interface
    virtual bool open(const char *url) override;
    virtual BoData read() override;
    virtual int64_t getTotalTime() override;

    virtual BoParameter getVideoParameter() override;
    virtual BoParameter getAudioParameter() override;

  private:
    AVFormatContext *ic = nullptr;

    int m_audioStream = 1;
    int m_videoStream = 0;
};

#endif // FFDEMUX_H