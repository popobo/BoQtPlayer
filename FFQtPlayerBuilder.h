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
    virtual std::shared_ptr<IResampler> createResampler() override;
    virtual std::shared_ptr<IPlayer> createPlayer() override;
    virtual std::shared_ptr<IFrameDispatcher> createFrameDispatcher() override;
};

#endif // FFQTPLAYERBUILDER_H
