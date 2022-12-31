#include "FFQtPlayer.h"
#include "BoLog.h"

FFQtPlayer::FFQtPlayer() {
    m_thread = std::make_shared<BoThread>();
}

bool FFQtPlayer::open(const char* url)
{
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

bool FFQtPlayer::start()
{
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
    m_thread->start();
    std::weak_ptr<FFQtPlayer> wself = shared_from_this();
    m_thread->addMainTask([wself]() {
        if (auto self = wself.lock()) {
            self->main();
        }
        });

    return true;
}

void FFQtPlayer::stop()
{
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

void FFQtPlayer::pause()
{
    std::unique_lock<std::mutex> locker(m_playerMutex);
    m_thread->pause();

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

void FFQtPlayer::resume()
{
    std::unique_lock<std::mutex> locker(m_playerMutex);
    m_thread->resume();

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

void FFQtPlayer::setVideoView(const std::shared_ptr<IVideoView>& newVideoView)
{
    m_videoView = newVideoView;
    m_videoView->open();
    m_videoDecoder->addObs(m_videoView);
}

void FFQtPlayer::setAudioPlayer(const std::shared_ptr<IAudioPlayer>& newAudioPlayer)
{
    m_audioPlayer = newAudioPlayer;
    m_audioPlayer->open(m_demux->getAudioParameter());
    m_resampler->addObs(m_audioPlayer);
}

bool FFQtPlayer::seek(double pos)
{
    if (!checkModulesValid()) {
        return false;
    }

    pause();
    std::unique_lock<std::mutex> locker(m_playerMutex);
    m_videoDecoder->clear();
    m_audioDecoder->clear();
    m_audioPlayer->clear();
    m_videoView->clear();

    bool ret = m_demux->seek(pos);//seek跳转到关键帧
    // 解码实际需要显示的帧
    int64_t seekPts = static_cast<int64_t>(pos * m_demux->getTotalTime());
    while (!m_thread->isExit()) {
        auto pkt = m_demux->read();
        if (pkt->size() <= 0) {
            break;
        }

        if (pkt->isAudio()) {
            if (pkt->pts() < seekPts) {
                continue;
            }
            // 将需要播放的音频写入缓冲队列
            m_demux->notify(pkt);
            continue;
        }

        //解码需要显示的帧之前的数据
        m_videoDecoder->sendPacket(pkt);
        auto data = m_videoDecoder->recvFrame();
        if (data->size() <= 0) {
            continue;
        }
        if (data->pts() >= seekPts) {
            break;
        }
    }
    locker.unlock();
    resume();
    return ret;
}

bool FFQtPlayer::checkModulesValid()
{
    if (!m_demux || !m_videoDecoder || !m_audioDecoder || !m_videoView || !m_resampler || !m_audioPlayer) {
        BO_ERROR("one of the modules is nullptr");
        return false;
    }

    return true;
}

double FFQtPlayer::getPlayPos()
{
    if (!checkModulesValid()) {
        return 0.0;
    }

    auto totalMs = m_demux->getTotalTime();
    auto currentPts = m_audioPlayer->getPts();
    if (totalMs == 0) {
        return 0.0;
    }
    return (double)currentPts / (double)totalMs;
}

void FFQtPlayer::main()
{
    std::unique_lock<std::mutex> locker(m_playerMutex);
    if (!m_audioPlayer || !m_videoDecoder) {
        locker.unlock();
        boSleep(1);
        return;
    }

    m_videoView->setSyncAudioPts(m_audioPlayer->getPts());

    locker.unlock();
    boSleep(1);
}
