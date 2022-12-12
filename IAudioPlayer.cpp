#include "IAudioPlayer.h"

IAudioPlayer::IAudioPlayer() {}

IAudioPlayer::~IAudioPlayer() {}

void IAudioPlayer::update(const std::shared_ptr<IBoData> &boData) {
    if (boData->size() <= 0 || !boData->data()) {
        return;
    }
    // 缓冲区满了则阻塞
    while (!m_isExit) {
        std::unique_lock<std::mutex> locker(m_framesMutex);
        if (m_frames.size() >= MAX_FRAME_COUNT) {
            locker.unlock();
            boSleep(1);
            continue;
        }
        m_frames.push_back(boData);
        break;
    }
}

std::shared_ptr<IBoData> IAudioPlayer::getData() {
    //因为IAudioPlay没启用线程, 所以m_isRuning没有置true,
    //按照XThread::stop()逻辑, m_isExit无法置为true
    //没有数据的时候阻塞
    m_isRunning = true;
    while (!m_isExit) {
        std::unique_lock<std::mutex> locker(m_framesMutex);
        if (!m_frames.empty()) {
            //有数据返回
            auto data = m_frames.front();
            m_frames.pop_front();
            m_pts = data->pts();
            return data;
        }
        locker.unlock();
        boSleep(1);
    }
    m_isRunning = false;

    return nullptr;
}

int IAudioPlayer::getPts() const { return m_pts; }

void IAudioPlayer::setPts(int newPts) { m_pts = newPts; }
