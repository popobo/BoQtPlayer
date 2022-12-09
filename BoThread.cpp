#include "BoThread.h"
#include "BoLog.h"
#include <thread>

BoThread::BoThread() {}

void BoThread::start() {
    m_isExit = false;
    std::thread th(&BoThread::threadMain, this);
    //当前线程放弃对新建线程的控制, 防止对象被清空时, 新建线程出错
    th.detach();
}

void BoThread::stop() {
    m_isExit = true;
    for (int i = 0; i < 200; ++i) {
        boSleep(1);
        if (!m_isRunning) {
            BO_INFO("stop thread successfully");
            return;
        }
        boSleep(1);
    }
    BO_INFO("stop thread time out");
}

void BoThread::threadMain() {
    BO_INFO("thread main begin");
    m_isRunning = true;
    main();
    m_isRunning = false;
    BO_INFO("thread main end");
}

void boSleep(int ms) {
    std::chrono::milliseconds sTime(ms);
    std::this_thread::sleep_for(sTime);
}
