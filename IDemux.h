#ifndef IDEMUX_H
#define IDEMUX_H

#include "BoData.h"
#include "BoThread.h"

//解封装接口类
class IDemux : public BoThread {
  public:
    //打开文件或者流媒体 rtmp http rtsp
    virtual bool open(const char *url) = 0;

    //读取一帧数据。数据由调用者清理
    virtual BoData read() = 0;

    //总时间 ms
    virtual int getTotalTime() = 0;

  protected:
    //不要让用户访问
    virtual void main();

  protected:
    int totalMs = 0;
};

#endif // IDEMUX_H
