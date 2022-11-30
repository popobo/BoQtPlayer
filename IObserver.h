#ifndef IOBSERVER_H
#define IOBSERVER_H

#include "BoData.h"
#include <memory>
#include <mutex>
#include <vector>

class IObserver {
  public:
    //观察者必须实现, 观察者接受数据函数,
    //主体调用观察者的update通知观察者已经收到数据了
    virtual void update(BoData boData) = 0;
};

#endif // IOBSERVER_H
