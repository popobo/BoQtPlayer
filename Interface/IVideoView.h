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

    virtual void pause() = 0;
    
    virtual void resume() = 0;

    virtual void setSyncAudioPts(long newSyncAudioPts) = 0;

    virtual void clear() = 0;
};

#endif // IVIDEOVIEW_H
