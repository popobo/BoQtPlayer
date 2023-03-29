#pragma once
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

    virtual bool isAudio() const override;
    virtual void setIsAudio(bool newIsAudio) override;

    virtual void update(std::shared_ptr<IBoData> boData) override;

    virtual void mainTask() override;

    virtual bool isSatisfied() override;

  private:
    AVCodecContext *m_codecContext = nullptr;
    std::mutex m_codecContextMutex;

    AVFrame *m_frame = nullptr;
    std::mutex m_frameMutex;

    bool m_isAudio = false;

    //读取缓冲
    const int32_t MAX_BODATA_QUEUE_SIZE = 100;
    const int32_t SATISFIED_BODATA_QUEUE_SIZE =
        static_cast<int32_t>(MAX_BODATA_QUEUE_SIZE * 0.75);
    const int32_t UNSATISFIED_BODATA_QUEUE_SIZE =
        static_cast<int32_t>(MAX_BODATA_QUEUE_SIZE * 0.5);
    std::queue<std::shared_ptr<IBoData>> m_boDataQueue;
    std::mutex m_boDataQueueMutex;

    double m_audioTimeBase = 0.0;
    double m_videoTimeBase = 0.0;

    std::atomic<bool> m_isStatified{false};
    std::atomic<bool> m_isDecodedDataLeftLastTime{false};
};
