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
    virtual bool open(const std::shared_ptr<IParameter> &parameter) override;
    virtual bool sendPacket(const std::shared_ptr<IBoData> &boData) override;
    virtual std::shared_ptr<IBoData> recvFrame() override;

    virtual void close() override;

    virtual void clear() override;

  private:
    AVCodecContext *m_codecContext = nullptr;
    AVFrame *m_frame = nullptr;
    std::mutex m_codecContextMutex;
};

#endif // FFDECODER_H
