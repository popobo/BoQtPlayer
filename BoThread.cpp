#include "BoThread.h"
#include "BoLog.h"
#include <thread>

BoThread::BoThread() : m_threaName{""} {}

BoThread::BoThread(const std::string &threadName) : m_threaName{threadName} {}

BoThread::~BoThread() {
    cancel_main_task();
    clearSubTasks();
    if (m_thread.joinable())
        m_thread.join();
}

bool BoThread::start() {
    m_isExit = false;
    m_isPaused = false;
    m_thread = std::thread(&BoThread::threadMain, this);
    //当前线程放弃对新建线程的控制, 防止对象被清空时, 新建线程出错
    return true;
}

void BoThread::stop() {
    BO_INFO("{0} stop", m_threaName);
    m_isExit = true;
}

bool BoThread::isPaused() { return m_isPaused; }

bool BoThread::isExit() { return m_isExit; }

void BoThread::pause() {
    BO_INFO("thread name: {0}", m_threaName);
    m_isPaused = true;
}

void BoThread::resume() {
    BO_INFO("thread name: {0}", m_threaName);
    m_isPaused = false;
}

void BoThread::set_main_task(const std::function<void()> &mainTask) {
    m_main_task = mainTask;
}

void BoThread::cancel_main_task() { m_main_task = nullptr; }

void BoThread::addSubTask(const std::function<void()> &subTask) {
    m_sub_tasks_queue.push(subTask);
}

void BoThread::clearSubTasks() { m_sub_tasks_queue.clean(); }

void BoThread::threadMain() {
    BO_INFO("thread main begin");
    while (!m_isExit) {
        {
            std::lock_guard lg{m_main_task_mutex};
            if (!m_isPaused && m_main_task) {
                m_main_task();
            }
        }

        while (auto sub_task_ptr = m_sub_tasks_queue.try_pop_front()) {
            (*sub_task_ptr)();
        }
        this_thread_sleep(1);
    }
    BO_INFO("thread main end");
}

void this_thread_sleep(int ms) {
    std::chrono::milliseconds sTime(ms);
    std::this_thread::sleep_for(sTime);
}
