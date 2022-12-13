#ifndef IVIDEOVIEW_H
#define IVIDEOVIEW_H

#include "IObserver.h"

class IVideoView : public IObserver {
  public:
    IVideoView();

    ~IVideoView(){};

    virtual bool open() = 0;

    virtual bool start() = 0;

    virtual void stop() = 0;

    virtual void initView(void *win) = 0;

    virtual void update(const std::shared_ptr<IBoData> &boData) override {}
};

#endif // IVIDEOVIEW_H
