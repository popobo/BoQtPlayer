#ifndef IAUDIOPLAY_H
#define IAUDIOPLAY_H

#include "BoThread.h"
#include "IObserver.h"
#include "IParameter.h"
#include "ISubject.h"
#include <queue>

class IAudioPlayer : public IObserver, public ISubject, public BoThread {
  public:
    IAudioPlayer();

    ~IAudioPlayer();

    virtual bool open(const std::shared_ptr<IParameter> &parameter) = 0;

    // 获取缓冲数据，如果没有则阻塞
    virtual std::shared_ptr<IBoData> getData() = 0;

    static const int MAX_FRAME_COUNT = 100;

    virtual long getPts() = 0;

    virtual const AudioOutputFormat &audioOutFormat() const;

    virtual void pause() = 0;

    virtual void resume() = 0;
    
    virtual void clear();

  protected:
    long m_pts{0};
    std::queue<std::shared_ptr<IBoData>> m_frames;
    std::mutex m_framesMutex;
    AudioOutputFormat m_audioOutFormat;
};

#endif // IAUDIOPLAY_H
