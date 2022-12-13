#ifndef IPLAYERBUILDER_H
#define IPLAYERBUILDER_H

#include "IPlayer.h"

class IPlayerBuilder {
  public:
    IPlayerBuilder();

    virtual ~IPlayerBuilder(){};

    virtual std::shared_ptr<IPlayer> buildPlayer();

  protected:
    virtual std::shared_ptr<IDemux> createDemux() = 0;
    virtual std::shared_ptr<IDecoder> createDecoder() = 0;
    virtual std::shared_ptr<IFrameDispatcher> createFrameDispathcer() = 0;
    virtual std::shared_ptr<IResampler> createResampler() = 0;
    virtual std::shared_ptr<IVideoView> createVideoView() = 0;
    virtual std::shared_ptr<IAudioPlayer> createAudioPlayer() = 0;
    virtual std::shared_ptr<IPlayer> createPlayer() = 0;
};

#endif // IPLAYERBUILDER_H
