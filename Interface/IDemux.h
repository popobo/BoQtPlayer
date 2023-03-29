#ifndef IDEMUX_H
#define IDEMUX_H

#include "BoThread.h"
#include "Data/IBoData.h"
#include "IParameter.h"
#include "ISubject.h"

//解封装接口类
class IDemux : public ISubject {
  public:
    virtual ~IDemux(){};

    //打开文件或者流媒体 rtmp http rtsp
    virtual bool open(const char *url) = 0;

    //读取一帧数据。数据由调用者清理
    virtual std::shared_ptr<IBoData> read() = 0;

    virtual bool seek(double pos) = 0;

    //总时间 ms
    virtual int64_t getTotalTime() = 0;

    virtual std::shared_ptr<IParameter> getVideoParameter() = 0;

    virtual std::shared_ptr<IParameter> getAudioParameter() = 0;

    virtual void mainTask() = 0;
};

#endif // IDEMUX_H
