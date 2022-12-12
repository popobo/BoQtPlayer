#ifndef QAUDIOPLAYER_H
#define QAUDIOPLAYER_H

#include "IAudioPlayer.h"
#include <QAudioDevice>
#include <QAudioSink>
#include <QIODevice>

class AudioBuffer;

class QAudioPlayer : public IAudioPlayer {
  public:
    QAudioPlayer();

    ~QAudioPlayer();

    virtual bool startPlay(BoParameter para) override;

    virtual void update(const std::shared_ptr<IBoData> &boData) override;

  private:
    bool m_isStarted{false};
    std::shared_ptr<AudioBuffer> m_ioDevice;
    std::shared_ptr<QAudioSink> m_audioSink;
    QAudioDevice m_defaultAudioDevice;
};

#endif // QAUDIOPLAYER_H