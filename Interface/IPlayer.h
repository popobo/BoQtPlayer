#ifndef IPLAYER_H
#define IPLAYER_H

#include "BoThread.h"
#include "IAudioPlayer.h"
#include "IDecoder.h"
#include "IDemux.h"
#include "IResampler.h"
#include "IVideoView.h"

// 使用前置声明有点，作为库提供给用户时，不需要提供头文件，减少头文件改动导致的编译事件

class IPlayer : public BoThread {
  public:
    IPlayer();

    virtual ~IPlayer(){};

    virtual bool open(const char *url);

    virtual bool start() override;

    virtual void stop() override;

    void setVideoView(const std::shared_ptr<IVideoView> &newVideoView);

    void setAudioPlayer(const std::shared_ptr<IAudioPlayer> &newAudioPlayer);

  public:
    std::shared_ptr<IDemux> m_demux;
    std::shared_ptr<IDecoder> m_videoDecoder;
    std::shared_ptr<IDecoder> m_audioDecoder;
    std::shared_ptr<IVideoView> m_videoView;
    std::shared_ptr<IResampler> m_resampler;
    std::shared_ptr<IAudioPlayer> m_audioPlayer;

  protected:
    void main() override;
    std::mutex m_playerMutex;
};

#endif // IPLAYER_H
