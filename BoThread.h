#ifndef BOTHREAD_H
#define BOTHREAD_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

void boSleep(int ms);

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

    void addMainTask(std::function<void()> mainTask);

    void clearMainTasks();

    void addSubTask(std::function<void()> subTask);

    void clearSubTasks();

  protected:
    std::atomic<bool> m_isExit{false};
    std::atomic<bool> m_isPaused{false};
    std::string m_threaName;

    std::vector<std::function<void()>> m_mainTasksVec;
    std::queue<std::function<void()>> m_subTasksQueue;
    std::mutex m_mainTasksVecMutex;
    std::mutex m_subTasksQueueMutex;

  private:
    void threadMain();
};

#endif // BOTHREAD_H
