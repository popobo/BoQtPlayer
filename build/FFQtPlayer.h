﻿#pragma once
#include "IPlayer.h"
class FFQtPlayer: 
    public IPlayer, public std::enable_shared_from_this<FFQtPlayer>
{
public:
    // 通过 IPlayer 继承
    virtual bool open(const char* url) override;
    virtual bool start() override;
    virtual void stop() override;
    virtual void pause() override;
    virtual void resume() override;
    virtual void setVideoView(const std::shared_ptr<IVideoView>& newVideoView) override;
    virtual void setAudioPlayer(const std::shared_ptr<IAudioPlayer>& newAudioPlayer) override;
    virtual bool seek(double pos) override;
    virtual bool checkModulesValid() override;
    virtual double getPlayPos() override;
    
protected:
    virtual void main() override;

private:
    
    std::mutex m_playerMutex;

    BoThread m_thread;
};

