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

    QAudioPlayer(QAudioDevice audioDevice, QAudioFormat audioFormat);

    ~QAudioPlayer();

    virtual bool startPlay(BoParameter para) override;

    virtual void update(const std::shared_ptr<IBoData> &boData) override;

    virtual void setAudioOutputParameter(const BoParameter &paraIn) override;

    const QAudioDevice &audioDevice() const;
    void setAudioDevice(const QAudioDevice &newAudioDevice);

  private:
    bool m_isStarted{false};
    std::shared_ptr<AudioBuffer> m_audioBuffer;
    std::shared_ptr<QAudioSink> m_audioSink;
    QAudioDevice m_audioDevice;
    QAudioFormat m_audioOutFormat;
};

#endif // QAUDIOPLAYER_H
