#ifndef IDECODER_H
#define IDECODER_H

#include "BoData.h"
#include "BoParameter.h"
#include "BoThread.h"
#include "IObserver.h"
#include "ISubject.h"
#include <list>

class IDecoder : public IObserver, public BoThread, public ISubject {
  public:
    IDecoder() {}

    virtual ~IDecoder() {}

    //打开解码器
    virtual bool open(const BoParameter &parameter) = 0;

    // feature模型，发送数据到线程解码
    virtual bool sendPacket(const BoData &boData) = 0;

    //从线程中获取解码结果, 并不会阻塞, 再次调用会复用上次空间, 线程不安全
    virtual BoData recvFrame() = 0;

    //由主体notify的数据 达到最大队列缓冲则阻塞
    // IObserver Interface
    virtual void update(BoData boData) override;

    // BoThread interface
    virtual void main() override;

    // 清理缓冲
    virtual void clear();

    virtual void close() = 0;

    bool isAudio() const;

    void setIsAudio(bool newIsAudio);

  public:
    const int MAX_LIST = 100;

  protected:
    bool m_isAudio = false;

    //读取缓冲
    std::list<BoData> m_boDataList;
    std::mutex m_boDataListMutex;
};

#endif // IDECODER_H
