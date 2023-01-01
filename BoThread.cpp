#include "BoThread.h"
#include "BoLog.h"
#include <thread>

BoThread::BoThread() :m_threaName{""} {}

BoThread::BoThread(const std::string& threadName) : m_threaName{threadName}
{
}

BoThread::~BoThread() {
    clearMainTasks();
    clearSubTasks();
}

bool BoThread::start() {
    m_isExit = false;
    m_isPaused = false;
    std::thread th(&BoThread::threadMain, this);
    //当前线程放弃对新建线程的控制, 防止对象被清空时, 新建线程出错
    th.detach();
    return true;
}

void BoThread::stop() {    
    m_isExit = true;
}

bool BoThread::isPaused()
{
    return m_isPaused;
}

bool BoThread::isExit()
{
    return m_isExit;
}

void BoThread::pause()
{
    BO_INFO("thread name: {0}", m_threaName);
    m_isPaused = true;
}

void BoThread::resume()
{
    BO_INFO("thread name: {0}", m_threaName);
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
    while (!m_isExit) {
        if (!m_isPaused) {
            m_mainTasksVecMutex.lock();
            for (const auto& mainTask : m_mainTasksVec) {
                mainTask();
            }
            m_mainTasksVecMutex.unlock();
        }
        
        m_subTasksQueueMutex.lock();
        while (!m_subTasksQueue.empty()) {
            auto subTask = m_subTasksQueue.front();
            subTask();
            m_subTasksQueue.pop();
        }
        m_subTasksQueueMutex.unlock();
        boSleep(1);
    }
    BO_INFO("thread main end");
}

void boSleep(int ms) {
    std::chrono::milliseconds sTime(ms);
    std::this_thread::sleep_for(sTime);
}
