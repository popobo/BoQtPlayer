#include "IPlayerBuilder.h"

IPlayerBuilder::IPlayerBuilder() {}

std::shared_ptr<IPlayer> IPlayerBuilder::buildPlayer() {
    std::shared_ptr<IDemux> demux = createDemux();
    std::shared_ptr<IDecoder> videoDecoder = createDecoder();
    std::shared_ptr<IDecoder> audioDecoder = createDecoder();
    std::shared_ptr<IResampler> resampler = createResampler();
    std::shared_ptr<IPlayer> player = createPlayer();

    demux->addObs(videoDecoder);
    demux->addObs(audioDecoder);
    audioDecoder->addObs(resampler);

    player->m_demux = demux;
    player->m_videoDecoder = videoDecoder;
    player->m_audioDecoder = audioDecoder;
    player->m_resampler = resampler;

    return player;
}
