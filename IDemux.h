#ifndef IDEMUX_H
#define IDEMUX_H

#include "BoData.h"
#include "BoParameter.h"
#include "BoThread.h"
#include "ISubject.h"

//解封装接口类
class IDemux : public BoThread, public ISubject {
  public:
    //打开文件或者流媒体 rtmp http rtsp
    virtual bool open(const char *url) = 0;

    //读取一帧数据。数据由调用者清理
    virtual std::shared_ptr<BoData> read() = 0;

    //总时间 ms
    virtual int64_t getTotalTime() = 0;

    virtual BoParameter getVideoParameter() = 0;

    virtual BoParameter getAudioParameter() = 0;

  protected:
    //不要让用户访问
    virtual void main();

  protected:
    int64_t totalMs = 0;
};

#endif // IDEMUX_H
