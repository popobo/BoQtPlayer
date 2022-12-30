#ifndef BOTHREAD_H
#define BOTHREAD_H

#include <memory>
#include <queue>
#include <vector>
#include <functional>
#include <mutex>

void boSleep(int ms);

class BoThread:public std::enable_shared_from_this<BoThread> {
  public:
    BoThread();

    virtual ~BoThread();

    //启动线程
    virtual bool start();

    //通过isExit变量安全停止线程(不一定成功),
    //在开发中不应该操作线程句柄直接让其停止, 风险大, 因为不知道程序执行到哪
    virtual void stop();

    //入口主函数
    virtual void main(){};

    virtual bool isPaused();

    virtual void pause();

    virtual void resume();

    virtual void addMainTask(std::function<void()> mainTask);

    virtual void clearMainTasks();

    virtual void addSubTask(std::function<void()> subTask);
    
    virtual void clearSubTasks();

  protected:
    bool m_isExit = false;
    bool m_isRunning = false;
    bool m_isPaused{false};
    
    std::vector<std::function<void()>> m_mainTasksVec;
    std::queue<std::function<void()>> m_subTasksQueue;
    std::mutex m_mainTasksVecMutex;
    std::mutex m_subTasksQueueMutex;

  private:
    void threadMain();
};

#endif // BOTHREAD_H
