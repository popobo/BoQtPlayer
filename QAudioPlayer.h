#pragma once

#include "BoThread.h"
#include "ElapsedTimer.h"
#include "IAudioPlayer.h"
#include <QAudioDevice>
#include <QAudioSink>
#include <QIODevice>
#include <QThread>

class AudioBuffer;

class QAudioPlayer : public QObject, public IAudioPlayer {
    Q_OBJECT

  public:
    QAudioPlayer();

    ~QAudioPlayer();

    virtual bool open(const std::shared_ptr<IParameter> &parameter) override;

    virtual void update(std::shared_ptr<IBoData> boData) override;

    // IAudioPlayer interface
    virtual bool start() override;

    virtual void stop() override;

    virtual void setBasePts(int64_t newPts) override;

    virtual long getPts() override;

    virtual std::shared_ptr<IBoData> getData() override;

    virtual void pause() override;

    virtual bool isPaused() override;

    virtual void resume() override;

    virtual void clear() override;

    virtual const AudioOutputFormat &audioOutFormat() const override;

    virtual bool isSatisfied() override;

  signals:
    void signalOpen();
    void signalStart();
    void signalStop();
    void signalPause();
    void signalResume();

  public slots:
    void slotOpen();
    void slotStart();
    void slotStop();
    void slotPause();
    void slotResume();

  private:
    bool m_isStarted{false};
    std::shared_ptr<AudioBuffer> m_audioBuffer;
    std::shared_ptr<QAudioSink> m_audioSink;

    QAudioDevice m_audioDevice;
    QAudioFormat m_qPreferedAudioFormat;
    double m_timeBase = 0.0;
    ElapsedTimer m_timer;

    std::atomic<bool> m_isPaused{false};
    std::atomic<bool> m_isExit{false};

    std::atomic<long> m_pts{0};
    std::atomic<long> m_basePts{0};
    std::atomic<long> m_fixPts{0};
    AudioOutputFormat m_audioOutFormat;

    QThread *m_audioPlayerThread;
    std::atomic<bool> m_isSatisfied = false;
};
