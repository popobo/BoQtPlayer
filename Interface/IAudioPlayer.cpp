#include "IAudioPlayer.h"
#include "BoThread.h"

IAudioPlayer::IAudioPlayer() {}

IAudioPlayer::~IAudioPlayer() {}

const AudioOutputFormat &IAudioPlayer::audioOutFormat() const {
    return m_audioOutFormat;
}
