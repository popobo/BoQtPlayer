#include "FFQtPlayer.h"
#include "BoLog.h"

FFQtPlayer::FFQtPlayer() {
    m_playerThread = std::make_shared<BoThread>();
    m_demuxThread = std::make_shared<BoThread>();
    m_videoDecoderThread = std::make_shared<BoThread>();
    m_audioDecoderThread = std::make_shared<BoThread>();
}

FFQtPlayer::~FFQtPlayer()
{
    BO_ERROR("");
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
    if (!areAllModulesValid()) {
        return false;
    }

    m_videoView->start();

    m_audioPlayer->start();

    m_videoDecoderThread->start();
    std::weak_ptr<IDecoder> weakVideoDecoder = m_videoDecoder;
    m_videoDecoderThread->addMainTask([weakVideoDecoder]() {
        if (auto videoDecoder = weakVideoDecoder.lock()) {
            videoDecoder->main();
        }
    });

    m_audioDecoderThread->start();
    std::weak_ptr<IDecoder> weakAudioDecoder = m_audioDecoder;
    m_audioDecoderThread->addMainTask([weakAudioDecoder]() {
        if (auto audioDecoder = weakAudioDecoder.lock()) {
            audioDecoder->main();
        }
    });

    m_demuxThread->start();
    std::weak_ptr<IDemux> weakDemux = m_demux;
    m_demuxThread->addMainTask([weakDemux]() {
        if (auto demux = weakDemux.lock()) {
            demux->main();
        }
    });

    m_playerThread->start();
    std::weak_ptr<IPlayer> wself = shared_from_this();
    m_playerThread->addMainTask([wself]() {
        if (auto self = wself.lock()) {
            self->main();
        }
    });

    return true;
}

void FFQtPlayer::stop()
{
    std::unique_lock<std::mutex> locker(m_playerMutex);
    if (!areAllModulesValid()) {
        return;
    }

    m_playerThread->stop();

    // （消费者）观察者先于（生产者）通知者停止，否则可能会因为有阻塞操作，导致线程退出超时
    m_audioPlayer->stop();

    m_videoView->stop();

    m_videoDecoderThread->stop();

    m_audioDecoderThread->stop();

    m_demuxThread->stop();
}

void FFQtPlayer::pause()
{
    std::unique_lock<std::mutex> locker(m_playerMutex);
    if (!areAllModulesValid()) {
        return;
    }

    m_playerThread->pause();

    m_demuxThread->pause();

    m_videoDecoderThread->pause();

    m_audioDecoderThread->pause();

    m_audioPlayer->pause();
    
    m_videoView->pause();
}

void FFQtPlayer::resume()
{
    std::unique_lock<std::mutex> locker(m_playerMutex);
    m_playerThread->resume();

    m_demuxThread->resume();

    m_videoDecoderThread->resume();

    m_audioDecoderThread->resume();

    m_audioPlayer->resume();

    m_videoView->resume();
}

void FFQtPlayer::setVideoView(const std::shared_ptr<IVideoView>& newVideoView)
{
    if (!newVideoView) {
        return;
    }

    std::unique_lock<std::mutex> locker(m_playerMutex);

    m_videoView = newVideoView;
    m_videoView->open();
    m_videoDecoder->addObs(m_videoView);
}

void FFQtPlayer::setAudioPlayer(const std::shared_ptr<IAudioPlayer>& newAudioPlayer)
{   
    if (!newAudioPlayer) {
        return;
    }

    std::unique_lock<std::mutex> locker(m_playerMutex);

    m_audioPlayer = newAudioPlayer;
    m_audioPlayer->open(m_demux->getAudioParameter());
    m_resampler->addObs(m_audioPlayer);
}

bool FFQtPlayer::seek(double pos)
{
    if (!areAllModulesValid()) {
        return false;
    }

    if (!areAllMoudlesPaused()) {
        pause();
    }
    
    // 等待所有相关模块暂停
    while (!areAllMoudlesPaused()) {
        boSleep(1);
    }

    std::unique_lock<std::mutex> locker(m_playerMutex);
    m_videoDecoder->clear();
    m_audioDecoder->clear();
    m_audioPlayer->clear();
    m_videoView->clear();

    bool ret = m_demux->seek(pos);//seek跳转到关键帧
    // 解码实际需要显示的帧
    int64_t seekPts = static_cast<int64_t>(pos * m_demux->getTotalTime());
    while (!m_playerThread->isExit()) {
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

bool FFQtPlayer::areAllModulesValid()
{
    if (!m_demux || !m_videoDecoder || !m_audioDecoder || !m_videoView || !m_resampler || !m_audioPlayer) {
        BO_ERROR("one of the modules is nullptr");
        return false;
    }

    if (!m_demuxThread || !m_videoDecoderThread || !m_audioDecoderThread || !m_playerThread) {
        BO_ERROR("one of the thread is nullptr");
        return false;
    }

    return true;
}

bool FFQtPlayer::areAllMoudlesPaused()
{
    if (!areAllModulesValid()) {
        return false;
    }

    if (!m_demuxThread->isPaused() || !m_videoDecoderThread->isPaused() || !m_audioDecoderThread->isPaused()) {
        BO_ERROR("one of the modules is nullptr");
        return false;
    }

    return true;
}

double FFQtPlayer::getPlayPos()
{
    if (!areAllModulesValid()) {
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
