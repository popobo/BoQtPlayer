#ifndef BOTHREAD_H
#define BOTHREAD_H

#include "bo_thread_safe_queue.h"
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

void this_thread_sleep(int ms);

class BoThread : public std::enable_shared_from_this<BoThread> {
  public:
    BoThread();

    BoThread(const std::string &threadName);

    virtual ~BoThread();

    //启动线程
    bool start();

    void stop();

    bool isPaused();

    bool isExit();

    void pause();

    void resume();

    void set_main_task(const std::function<void()> &mainTask);

    void cancel_main_task();

    void addSubTask(const std::function<void()> &subTask);

    void clearSubTasks();

  protected:
    std::atomic<bool> m_isExit{false};
    std::atomic<bool> m_isPaused{false};
    std::string m_threaName;

    std::mutex m_main_task_mutex;
    std::function<void()> m_main_task;
    bo_thread_safe_queue<std::function<void()>> m_sub_tasks_queue;

  private:
    void threadMain();
};

#endif // BOTHREAD_H
