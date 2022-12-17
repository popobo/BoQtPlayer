#include "IPlayerBuilder.h"

IPlayerBuilder::IPlayerBuilder() {}

std::shared_ptr<IPlayer> IPlayerBuilder::buildPlayer() {
    std::shared_ptr<IDemux> demux = createDemux();
    std::shared_ptr<IDecoder> videoDecoder = createDecoder();
    std::shared_ptr<IDecoder> audioDecoder = createDecoder();
    std::shared_ptr<IResampler> resampler = createResampler();
    std::shared_ptr<IPlayer> player = createPlayer();
    auto frameDispatcher = createFrameDispatcher();

    demux->addObs(videoDecoder);
    demux->addObs(audioDecoder);
    // audioDecoder->addObs(resampler);
    audioDecoder->addObs(frameDispatcher);
    videoDecoder->addObs(frameDispatcher);

    player->m_demux = demux;
    player->m_videoDecoder = videoDecoder;
    player->m_audioDecoder = audioDecoder;
    player->m_resampler = resampler;
    player->m_frameDispatcher = frameDispatcher;

    return player;
}
