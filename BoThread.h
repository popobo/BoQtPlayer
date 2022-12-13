#ifndef BOTHREAD_H
#define BOTHREAD_H

void boSleep(int ms);

class BoThread {
  public:
    BoThread();

    ~BoThread() {}

    //启动线程
    virtual bool start();

    //通过isExit变量安全停止线程(不一定成功),
    //在开发中不应该操作线程句柄直接让其停止, 风险大, 因为不知道程序执行到哪
    virtual void stop();

    //入口主函数
    virtual void main(){};

  protected:
    bool m_isExit = false;
    bool m_isRunning = false;

  private:
    void threadMain();
};

#endif // BOTHREAD_H
