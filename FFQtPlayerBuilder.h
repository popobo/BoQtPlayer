#ifndef FFQTPLAYERBUILDER_H
#define FFQTPLAYERBUILDER_H

#include "IPlayerBuilder.h"

class FFQtPlayerBuilder : public IPlayerBuilder {
  public:
    FFQtPlayerBuilder();

    ~FFQtPlayerBuilder();

  protected:
    // IPlayerBuilder interface
    virtual std::shared_ptr<IDemux> createDemux() override;
    virtual std::shared_ptr<IDecoder> createDecoder() override;
    virtual std::shared_ptr<IFrameDispatcher> createFrameDispathcer() override;
    virtual std::shared_ptr<IResampler> createResampler() override;
    virtual std::shared_ptr<IAudioPlayer> createAudioPlayer() override;
    virtual std::shared_ptr<IPlayer> createPlayer() override;
};

#endif // FFQTPLAYERBUILDER_H
