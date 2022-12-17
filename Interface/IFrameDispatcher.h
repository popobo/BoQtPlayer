#ifndef IFRAMEDISPATCHER_H
#define IFRAMEDISPATCHER_H

#include "BoThread.h"
#include "IObserver.h"
#include "IParameter.h"
#include "ISubject.h"

#include <queue>

class IFrameDispatcher : public BoThread, public IObserver, public ISubject {
  public:
    IFrameDispatcher();

    bool open(std::shared_ptr<IParameter> audioPara,
              std::shared_ptr<IParameter> videoPara);

    virtual void update(const std::shared_ptr<IBoData> &boData) override;

  protected:
    virtual void main() override;
    std::queue<std::shared_ptr<IBoData>> m_audioFrames;
    std::queue<std::shared_ptr<IBoData>> m_videoFrames;
    double m_currentVideoPts = 0;
    double m_currentAudioPts = 0;
    std::shared_ptr<IParameter> m_audioPara;
    std::shared_ptr<IParameter> m_videoPara;
};

#endif // IFRAMEDISPATCHER_H
