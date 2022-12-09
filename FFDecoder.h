#ifndef FFDECODER_H
#define FFDECODER_H

#include "IDecoder.h"
extern "C" {
#include "libavcodec/avcodec.h"
}

class FFDecoder : public IDecoder {
  public:
    FFDecoder();

    ~FFDecoder();

    // IDecoder interface
    virtual bool open(const BoParameter &parameter) override;
    virtual bool sendPacket(const BoData &boData) override;
    virtual BoData recvFrame() override;

    virtual void clear() override;
    virtual void close() override;

  private:
    AVCodecContext *m_codecContext = nullptr;
    AVFrame *m_frame = nullptr;
    std::mutex m_codecContextMutex;
};

#endif // FFDECODER_H
