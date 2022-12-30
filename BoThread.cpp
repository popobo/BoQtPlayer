#include "BoThread.h"
#include "BoLog.h"
#include <thread>

BoThread::BoThread() {}

BoThread::~BoThread() {
    m_mainTasksVec.clear();
    m_subTasksQueue = {};
}

bool BoThread::start() {
    m_isExit = false;
    m_isPaused = false;
    std::thread th(&BoThread::threadMain, this);
    //当前线程放弃对新建线程的控制, 防止对象被清空时, 新建线程出错
    th.detach();
    // this的野指针的可能性
    addMainTask([this]() {
        this->main();
    });
    return true;
}

void BoThread::stop() {
    m_isExit = true;
    for (int i = 0; i < 200; ++i) {
        boSleep(1);
        if (!m_isRunning) {
            BO_INFO("stop thread successfully");
            m_mainTasksVec.clear();
            return;
        }
        boSleep(1);
    }
    BO_INFO("stop thread time out");
}

bool BoThread::isPaused()
{
    return m_isPaused;
}

void BoThread::pause()
{
    m_isPaused = true;
}

void BoThread::resume()
{
    m_isPaused = false;
}

void BoThread::addMainTask(std::function<void()> mainTask)
{
    std::unique_lock<std::mutex> locker{ m_mainTasksVecMutex };
    m_mainTasksVec.push_back(mainTask);
}

void BoThread::clearMainTasks()
{
    std::unique_lock<std::mutex> locker{ m_mainTasksVecMutex };
    m_mainTasksVec.clear();
}

void BoThread::addSubTask(std::function<void()> subTask)
{
    std::unique_lock<std::mutex> locker{ m_subTasksQueueMutex };
    m_subTasksQueue.push(subTask);
}

void BoThread::clearSubTasks()
{
    std::unique_lock<std::mutex> locker{ m_subTasksQueueMutex };
    m_subTasksQueue = {};
}

void BoThread::threadMain() {
    BO_INFO("thread main begin");
    m_isRunning = true;
    while (!m_isExit) {
        for (const auto& mainTask : m_mainTasksVec) {
            mainTask();
        }
        
        while (!m_subTasksQueue.empty()) {
            auto subTask = m_subTasksQueue.front();
            subTask();
            m_subTasksQueue.pop();
        }
    }
    m_isRunning = false;
    BO_INFO("thread main end");
}

void boSleep(int ms) {
    std::chrono::milliseconds sTime(ms);
    std::this_thread::sleep_for(sTime);
}
