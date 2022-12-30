#include "IAudioPlayer.h"
#include "BoThread.h"

IAudioPlayer::IAudioPlayer() {}

IAudioPlayer::~IAudioPlayer() {}

const AudioOutputFormat &IAudioPlayer::audioOutFormat() const {
    return m_audioOutFormat;
}

void IAudioPlayer::clear()
{
    std::unique_lock<std::mutex> locker{ m_framesMutex };
    m_frames = {};
}
