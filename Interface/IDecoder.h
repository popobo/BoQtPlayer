#ifndef IDECODER_H
#define IDECODER_H

#include "BoThread.h"
#include "Data/IBoData.h"
#include "IObserver.h"
#include "IParameter.h"
#include "ISubject.h"
#include <queue>

class IDecoder : public IObserver, public ISubject {
  public:
    IDecoder() {}

    virtual ~IDecoder() {}

    //打开解码器
    virtual bool open(const std::shared_ptr<IParameter> &parameter) = 0;

    // feature模型，发送数据到线程解码
    virtual bool sendPacket(const std::shared_ptr<IBoData> &boData) = 0;

    //从线程中获取解码结果, 并不会阻塞, 再次调用会复用上次空间, 线程不安全
    virtual std::shared_ptr<IBoData> recvFrame() = 0;

    //由主体notify的数据 达到最大队列缓冲则阻塞
    // IObserver Interface
    virtual void update(const std::shared_ptr<IBoData> &boData) = 0;

    // BoThread interface
    virtual void close() = 0;

    virtual void clear() = 0;

    virtual bool isAudio() const = 0;

    virtual void setIsAudio(bool newIsAudio) = 0;

    virtual void mainTask() = 0;
};

#endif // IDECODER_H
