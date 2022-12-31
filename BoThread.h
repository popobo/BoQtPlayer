#ifndef BOTHREAD_H
#define BOTHREAD_H

#include <memory>
#include <queue>
#include <vector>
#include <functional>
#include <mutex>

void boSleep(int ms);

class BoThread: public std::enable_shared_from_this<BoThread> {
  public:
    BoThread();

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
    bool m_isExit{ false };
    bool m_isPaused{ false };
    
    std::vector<std::function<void()>> m_mainTasksVec;
    std::queue<std::function<void()>> m_subTasksQueue;
    std::mutex m_mainTasksVecMutex;
    std::mutex m_subTasksQueueMutex;

  private:
    void threadMain();

    void _stop();

    void _pause();

    void _resume();
};

#endif // BOTHREAD_H
