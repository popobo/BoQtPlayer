#ifndef QAUDIOPLAYER_H
#define QAUDIOPLAYER_H

#include "BoThread.h"
#include "IAudioPlayer.h"
#include <QAudioDevice>
#include <QAudioSink>
#include <QIODevice>

class AudioBuffer;

class QAudioPlayer : public IAudioPlayer,
                     public std::enable_shared_from_this<QAudioPlayer> {
  public:
    QAudioPlayer();

    ~QAudioPlayer();

    virtual bool open(const std::shared_ptr<IParameter> &parameter) override;

    virtual void update(const std::shared_ptr<IBoData> &boData) override;

    // IAudioPlayer interface
    virtual bool start() override;
    virtual void stop() override;

    virtual std::shared_ptr<IBoData> getData() override;

  protected:
    virtual void main() override;

  private:
    bool m_isStarted{false};
    std::shared_ptr<AudioBuffer> m_audioBuffer;
    std::shared_ptr<QAudioSink> m_audioSink;
    QAudioDevice m_audioDevice;
    QAudioFormat m_qPreferedAudioFormat;
    double m_timeBase = 0.0;
};

#endif // QAUDIOPLAYER_H
