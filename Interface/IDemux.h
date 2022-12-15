#ifndef IDEMUX_H
#define IDEMUX_H

#include "FFParameter.h"
#include "BoThread.h"
#include "Data/IBoData.h"
#include "ISubject.h"

//解封装接口类
class IDemux : public BoThread, public ISubject {
  public:
    ~IDemux();

    //打开文件或者流媒体 rtmp http rtsp
    virtual bool open(const char *url) = 0;

    //读取一帧数据。数据由调用者清理
    virtual std::shared_ptr<IBoData> read() = 0;

    //总时间 ms
    virtual int64_t getTotalTime() = 0;

    virtual FFParameter getVideoParameter() = 0;

    virtual FFParameter getAudioParameter() = 0;

  protected:
    //不要让用户访问
    virtual void main();

  protected:
    int64_t totalMs = 0;
};

#endif // IDEMUX_H
