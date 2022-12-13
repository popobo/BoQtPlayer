#ifndef IFRAMEDISPATCHER_H
#define IFRAMEDISPATCHER_H

#include "IObserver.h"
#include "ISubject.h"

class IFrameDispatcher : public IObserver, public ISubject {
  public:
    IFrameDispatcher();

    ~IFrameDispatcher(){};

    virtual void update(const std::shared_ptr<IBoData> &boData) override;

    virtual void dispatchData() = 0;
};

#endif // IFRAMEDISPATCHER_H
