#ifndef FFDECODE_H
#define FFDECODE_H

#include "IDecode.h"
extern "C" {
#include "libavcodec/avcodec.h"
}

class FFDecode : public IDecode {
  public:
    FFDecode();

    // IDecode interface
    virtual bool open(const BoParameter &parameter) override;
    virtual bool sendPacket(const BoData &boData) override;
    virtual BoData recvFrame() override;

  private:
    AVCodecContext *m_codecContext = nullptr;
    AVFrame *m_frame = nullptr;
};

#endif // FFDECODE_H
