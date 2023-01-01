#ifndef QAUDIOPLAYER_H
#define QAUDIOPLAYER_H

#include "BoThread.h"
#include "ElapsedTimer.h"
#include "IAudioPlayer.h"
#include <QAudioDevice>
#include <QAudioSink>
#include <QIODevice>

class AudioBuffer;

class QAudioPlayer : public IAudioPlayer {
  public:
    QAudioPlayer();

    ~QAudioPlayer();

    virtual bool open(const std::shared_ptr<IParameter> &parameter) override;

    virtual void update(const std::shared_ptr<IBoData> &boData) override;

    // IAudioPlayer interface
    virtual bool start() override;

    virtual void stop() override;

    virtual long getPts() override;

    virtual std::shared_ptr<IBoData> getData() override;

    virtual void pause() override;

    virtual bool isPaused() override;

    virtual void resume() override;

    virtual void clear() override;

    virtual const AudioOutputFormat& audioOutFormat() const override;

  private:
    bool m_isStarted{false};
    std::shared_ptr<AudioBuffer> m_audioBuffer;
    std::shared_ptr<QAudioSink> m_audioSink;
    QAudioDevice m_audioDevice;
    QAudioFormat m_qPreferedAudioFormat;
    double m_timeBase = 0.0;
    ElapsedTimer m_timer;
    std::atomic<bool> m_isPaused{ false };
    std::atomic<bool> m_isExit{ false };

    long m_pts{ 0 };
    AudioOutputFormat m_audioOutFormat;

    // 通过 IAudioPlayer 继承
    virtual bool isSatisfied() override;
};

#endif // QAUDIOPLAYER_H
