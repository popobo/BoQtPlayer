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

    if (!m_resampler || !m_resampler->open(m_demux->getAudioParameter(),
                                           m_audioPlayer->audioOutFormat())) {
        BO_ERROR("m_resampler failed to open");
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

    if (!m_audioPlayer || !m_audioPlayer->start()) {
        BO_ERROR("m_demux failed to start");
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

    locker.unlock();
    BoThread::start();
    return true;
}

void IPlayer::stop() {
    std::unique_lock<std::mutex> locker(m_playerMutex);

    // （消费者）观察者先于（生产者）通知者停止，否则可能会因为有阻塞操作，导致线程退出超时
    if (m_audioPlayer) {
        m_audioPlayer->stop();
    }

    if (m_videoView) {
        m_videoView->stop();
    }

    if (m_videoDecoder) {
        m_videoDecoder->stop();
    }

    if (m_audioDecoder) {
        m_audioDecoder->stop();
    }

    if (m_demux) {
        m_demux->stop();
    }
}

void IPlayer::pause()
{
    std::unique_lock<std::mutex> locker(m_playerMutex);
    BoThread::pause();

    if (m_demux) {
        m_demux->pause();
    }

    if (m_videoDecoder) {
        m_videoDecoder->pause();
    }

    if (m_audioDecoder) {
        m_audioDecoder->pause();
    }

    if (m_audioPlayer) {
        m_audioPlayer->pause();
    }

    if (m_videoView) {
        m_videoView->pause();
    }
}

void IPlayer::resume()
{
    std::unique_lock<std::mutex> locker(m_playerMutex);
    BoThread::resume();

    if (m_demux) {
        m_demux->resume();
    }

    if (m_videoDecoder) {
        m_videoDecoder->resume();
    }

    if (m_audioDecoder) {
        m_audioDecoder->resume();
    }
    
    if (m_audioPlayer) {
        m_audioPlayer->resume();
    }

    if (m_videoView) {
        m_videoView->resume();
    }
}


// 这两者时是依赖平台的，有调用者传入
void IPlayer::setVideoView(const std::shared_ptr<IVideoView> &newVideoView) {
    m_videoView = newVideoView;
    m_videoView->open();
    m_videoDecoder->addObs(m_videoView);
}

void IPlayer::setAudioPlayer(
    const std::shared_ptr<IAudioPlayer> &newAudioPlayer) {
    m_audioPlayer = newAudioPlayer;
    m_audioPlayer->open(m_demux->getAudioParameter());
    m_resampler->addObs(m_audioPlayer);
}

void IPlayer::seek(double pos)
{
    if (!m_demux) {
        BO_ERROR("m_demux is nullptr");
        return;
    }
    pause();
}

void IPlayer::main() {
    while (!m_isExit) {
        if (m_isPaused) {
            boSleep(1);
            continue;
        }
        
        std::unique_lock<std::mutex> locker(m_playerMutex);
        if (!m_audioPlayer || !m_videoDecoder) {
            locker.unlock();
            boSleep(1);
            continue;
        }

        m_videoView->setSyncAudioPts(m_audioPlayer->getPts());

        locker.unlock();
        boSleep(1);
    }
}
