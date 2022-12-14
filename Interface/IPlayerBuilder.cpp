#include "IPlayerBuilder.h"

IPlayerBuilder::IPlayerBuilder() {}

std::shared_ptr<IPlayer> IPlayerBuilder::buildPlayer() {
    std::shared_ptr<IDemux> demux = createDemux();
    std::shared_ptr<IDecoder> videoDecoder = createDecoder();
    std::shared_ptr<IDecoder> audioDecoder = createDecoder();
    std::shared_ptr<IFrameDispatcher> frameDispatcher = createFrameDispathcer();
    std::shared_ptr<IResampler> resampler = createResampler();
    std::shared_ptr<IAudioPlayer> audioPlayer = createAudioPlayer();
    std::shared_ptr<IPlayer> player = createPlayer();

    demux->addObs(videoDecoder);
    demux->addObs(audioDecoder);
    videoDecoder->addObs(frameDispatcher);
    frameDispatcher->addObs(videoView);
    audioDecoder->addObs(resampler);
    resampler->addObs(audioPlayer);

    player->m_demux = demux;
    player->m_videoDecoder = videoDecoder;
    player->m_audioDecoder = audioDecoder;
    player->m_frameDispatcher = frameDispatcher;
    player->m_videoView = videoView;
    player->m_resampler = resampler;
    player->m_audioPlayer = audioPlayer;

    return player;
}
