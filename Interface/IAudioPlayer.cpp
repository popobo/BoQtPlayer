#include "IAudioPlayer.h"
#include "BoThread.h"

IAudioPlayer::IAudioPlayer() {}

IAudioPlayer::~IAudioPlayer() {}

int IAudioPlayer::getPts() const { return m_pts; }

void IAudioPlayer::setPts(int newPts) { m_pts = newPts; }

const AudioOutputFormat &IAudioPlayer::audioOutFormat() const {
    return m_audioOutFormat;
}
