#include "IPlayer.h"
#include "BoLog.h"

IPlayer::IPlayer() {}

bool IPlayer::open(const char *url) {
    std::unique_lock<std::mutex> locker(m_playerMutex);
    if (!m_demux || !m_demux->open(url)) {
        BO_ERROR("m_demux failed to open {0}", url);
        return false;
    }

    if (!m_videoDecoder ||
        !m_videoDecoder->open(m_demux->getVideoParameter())) {
        BO_ERROR("m_videoDecoder failed to open {0}", url);
        return false;
    }

    if (!m_audioDecoder ||
        !m_audioDecoder->open(m_demux->getAudioParameter())) {
        BO_ERROR("m_audioDecoder failed to open {0}", url);
        return false;
    }

    if (!m_audioPlayer || m_audioPlayer->open()) {
        BO_ERROR("m_audioPlayer failed to open");
        return false;
    }

    if (!m_resampler || m_resampler->open(m_demux->getVideoParameter(),
                                          m_audioPlayer->audioOutFormat())) {
        BO_ERROR("m_resampler failed to open");
        return false;
    }

    if (!m_videoView || !m_videoView->open()) {
        BO_ERROR("m_videoView failed to open");
        return false;
    }

    return true;
}

bool IPlayer::start() {
    std::unique_lock<std::mutex> locker(m_playerMutex);

    if (!m_videoView || !m_videoView->start()) {
        BO_ERROR("m_videoView failed to start");
        return false;
    }

    if (!m_videoDecoder || !m_videoDecoder->start()) {
        BO_ERROR("m_videoDecoder failed to start");
        return false;
    }

    if (!m_audioDecoder || !m_audioDecoder->start()) {
        BO_ERROR("m_audioDecoder failed to start");
        return false;
    }

    if (!m_demux || !m_demux->start()) {
        BO_ERROR("m_demux failed to start");
        return false;
    }

    if (!m_audioPlayer || !m_audioPlayer->start()) {
        BO_ERROR("m_demux failed to start");
        return false;
    }

    locker.unlock();
    BoThread::start();
    return true;
}

void IPlayer::stop() {
    std::unique_lock<std::mutex> locker(m_playerMutex);

    if (m_demux) {
        m_demux->stop();
    }

    if (m_videoDecoder) {
        m_videoDecoder->stop();
    }

    if (m_audioDecoder) {
        m_audioDecoder->stop();
    }

    if (m_videoView) {
        m_videoView->stop();
    }

    if (m_audioPlayer) {
        m_audioPlayer->stop();
    }
}

// 这两者时是依赖平台的，有调用者传入
void IPlayer::setVideoView(const std::shared_ptr<IVideoView> &newVideoView) {
    m_videoView = newVideoView;
    m_videoDecoder->addObs(m_videoView);
}

void IPlayer::setAudioPlayer(
    const std::shared_ptr<IAudioPlayer> &newAudioPlayer) {
    m_audioPlayer = newAudioPlayer;
    m_resampler->addObs(m_audioPlayer);
}
