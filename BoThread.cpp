#include "BoThread.h"
#include "BoLog.h"
#include <thread>

BoThread::BoThread() {}

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
    std::weak_ptr<BoThread> wself = shared_from_this();
    addSubTask([wself]() {
        if (auto self = wself.lock()) {
            self->_stop();
        }
        });
}

bool BoThread::isPaused()
{
    return m_isPaused;
}

void BoThread::pause()
{
    std::weak_ptr<BoThread> wself = shared_from_this();
    addSubTask([wself]() {
        if (auto self = wself.lock()) {
            self->_pause();
        }
        });
}

void BoThread::resume()
{
    std::weak_ptr<BoThread> wself = shared_from_this();
    addSubTask([wself]() {
        if (auto self = wself.lock()) {
            self->_resume();
        }
        });
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
        if (m_isPaused) {
            boSleep(1);
            continue;
        }

        m_mainTasksVecMutex.lock();
        for (const auto& mainTask : m_mainTasksVec) {
            mainTask();
        }
        m_mainTasksVecMutex.unlock();

        m_subTasksQueueMutex.lock();
        while (!m_subTasksQueue.empty()) {
            auto subTask = m_subTasksQueue.front();
            subTask();
            m_subTasksQueue.pop();
        }
        m_subTasksQueueMutex.unlock();
    }
    BO_INFO("thread main end");
}

void BoThread::_stop()
{
    m_isExit = true;
}

void BoThread::_pause()
{
    m_isPaused = true;
}

void BoThread::_resume()
{
    m_isPaused = false;
}

void boSleep(int ms) {
    std::chrono::milliseconds sTime(ms);
    std::this_thread::sleep_for(sTime);
}
