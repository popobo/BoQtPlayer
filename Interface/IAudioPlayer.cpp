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

bool IAudioPlayer::start() {
    m_isExit = false;
    return true;
}

void IAudioPlayer::stop() { m_isExit = true; }

std::shared_ptr<IBoData> IAudioPlayer::getData() {
    //没有数据的时候阻塞
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

    return nullptr;
}

int IAudioPlayer::getPts() const { return m_pts; }

void IAudioPlayer::setPts(int newPts) { m_pts = newPts; }

const AudioOutputFormat &IAudioPlayer::audioOutFormat() const {
    return m_audioOutFormat;
}
