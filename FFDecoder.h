#ifndef FFDECODER_H
#define FFDECODER_H

#include "IDecoder.h"
extern "C" {
#include "libavcodec/avcodec.h"
}

class FFDecoder : public IDecoder, public std::enable_shared_from_this<FFDecoder> {
  public:
    FFDecoder();

    ~FFDecoder();

    // IDecoder interface
    virtual bool open(const std::shared_ptr<IParameter> &parameter) override;
    virtual bool sendPacket(const std::shared_ptr<IBoData> &boData) override;
    virtual std::shared_ptr<IBoData> recvFrame() override;

    virtual void close() override;

    virtual void clear() override;

    virtual bool isAudio() const override;
    virtual void setIsAudio(bool newIsAudio) override;
    
    virtual void update(const std::shared_ptr<IBoData>& boData) override;

    virtual void main() override;


  private:
    const int MAX_LIST = 100;

    AVCodecContext *m_codecContext = nullptr;
    std::mutex m_codecContextMutex;
    
    AVFrame *m_frame = nullptr;
    std::mutex m_frameMutex;

    bool m_isAudio = false;

    //读取缓冲
    std::queue<std::shared_ptr<IBoData>> m_boDataQueue;
    std::mutex m_boDataQueueMutex;

    double m_audioTimeBase = 0.0;
    double m_videoTimeBase = 0.0;

    std::shared_ptr<BoThread> m_thread;
};

#endif // FFDECODER_H
