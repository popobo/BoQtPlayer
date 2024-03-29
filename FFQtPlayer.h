﻿#pragma once
#include "IPlayer.h"
class FFQtPlayer : public IPlayer,
                   public std::enable_shared_from_this<FFQtPlayer> {
  public:
    FFQtPlayer();

    virtual ~FFQtPlayer();

    // 通过 IPlayer 继承
    virtual bool open(const char *url) override;
    virtual bool start() override;
    virtual void stop() override;
    virtual void pause() override;
    virtual void resume() override;
    virtual void
    setVideoView(const std::shared_ptr<IVideoView> &newVideoView) override;
    virtual void setAudioPlayer(
        const std::shared_ptr<IAudioPlayer> &newAudioPlayer) override;
    virtual bool seek(double pos) override;
    virtual double getPlayPos() override;
    virtual bool _seek(double pos) override;

  private:
    bool areAllModulesValid();
    bool areAllModulesPaused();

  public:
    virtual void mainTask() override;

  private:
    std::mutex m_playerMutex;

    std::shared_ptr<BoThread> m_playerThread;
    std::shared_ptr<BoThread> m_demuxThread;
    std::shared_ptr<BoThread> m_videoDecoderThread;
    std::shared_ptr<BoThread> m_audioDecoderThread;
};
