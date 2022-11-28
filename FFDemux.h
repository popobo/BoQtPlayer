#ifndef FFDEMUX_H
#define FFDEMUX_H

#include "IDemux.h"

struct AVFormatContext;

class FFDemux : public IDemux {
  public:
    FFDemux();

    // IDemux interface
    virtual bool open(const char *url) override;
    virtual BoData read() override;
    virtual int getTotalTime() override;

  private:
    AVFormatContext *ic = nullptr;
};

#endif // FFDEMUX_H
