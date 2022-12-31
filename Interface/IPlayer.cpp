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

    std::weak_ptr<IDemux> weakDemux = m_demux;
    m_demux->addSubTask([weakDemux]() {
        if (auto demux = weakDemux.lock()) {
            demux->pause();
        }
    });

    std::weak_ptr<IDecoder> weakVideoDecoder = m_videoDecoder;
    m_videoDecoder->addSubTask([weakVideoDecoder]() {
        if (auto videoDecoder = weakVideoDecoder.lock()) {
            videoDecoder->pause();
        }
    });

    std::weak_ptr<IDecoder> weakAudioDecoder = m_audioDecoder;
    m_audioDecoder->addSubTask([weakAudioDecoder]() {
        if (auto audioDecoder = weakAudioDecoder.lock()) {
            audioDecoder->pause();
        }
    });

    if (m_audioPlayer) {
        m_audioPlayer->pause();
    }

    if (m_videoView) {
        m_videoView->pause();
    }
}

void IPlayer::resume()
{
    if (!checkModulesValid()) {
        return;
    }

    std::unique_lock<std::mutex> locker(m_playerMutex);
    BoThread::resume();

    std::weak_ptr<IDemux> weakDemux = m_demux;
    m_demux->addSubTask([weakDemux]() {
        if (auto demux = weakDemux.lock()) {
            demux->resume();
        }
    });

    std::weak_ptr<IDecoder> weakVideoDecoder = m_videoDecoder;
    m_videoDecoder->addSubTask([weakVideoDecoder]() {
        if (auto videoDecoder = weakVideoDecoder.lock()) {
            videoDecoder->resume();
        }
    });

    std::weak_ptr<IDecoder> weakAudioDecoder = m_audioDecoder;
    m_audioDecoder->addSubTask([weakAudioDecoder]() {
        if(auto audioDecoder = weakAudioDecoder.lock()){
            audioDecoder->resume();
        }
    });

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

bool IPlayer::seek(double pos)
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
    while (!m_isExit) {
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

bool IPlayer::checkModulesValid()
{
    if (!m_demux || !m_videoDecoder || !m_audioDecoder || !m_videoView || !m_resampler || !m_audioPlayer) {
        BO_ERROR("one of the modules is nullptr");
        return false;
    }
    
    return true;
}

double IPlayer::getPlayPos()
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

void IPlayer::main() {
    if (m_isPaused) {
        boSleep(1);
        return;
    }
        
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
