#ifndef IAUDIOPLAY_H
#define IAUDIOPLAY_H

#include "BoParameter.h"
#include "BoThread.h"
#include "IObserver.h"
#include "ISubject.h"
#include <list>

class IAudioPlayer : public BoThread, public IObserver, public ISubject {
  public:
    IAudioPlayer();

    ~IAudioPlayer();

    // 缓冲满后阻塞
    virtual void update(const std::shared_ptr<IBoData> &boData) override;

    virtual bool startPlay(BoParameter para) = 0;

    // 获取缓冲数据，如果没有则阻塞
    virtual std::shared_ptr<IBoData> getData();

    static const int MAX_FRAME_COUNT = 100;

    int getPts() const;
    void setPts(int newPts);

    bool isExist() const;
    void setIsExist(bool newIsExist);

    virtual void setAudioOutputParameter(const BoParameter &paraIn) = 0;

  private:
    int m_pts{0};
    std::list<std::shared_ptr<IBoData>> m_frames;
    std::mutex m_framesMutex;
};

#endif // IAUDIOPLAY_H
