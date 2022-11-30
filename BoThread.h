#ifndef BOTHREAD_H
#define BOTHREAD_H

void boSleep(int ms);

class BoThread {
  public:
    BoThread();

    //启动线程
    virtual void start();

    //通过isExit变量安全停止线程(不一定成功),
    //在开发中不应该操作线程句柄直接让其停止, 风险大, 因为不知道程序执行到哪
    virtual void stop();

    //入口主函数
    virtual void main(){};

    void threadMain();

  protected:
    bool m_isExit = false;
    bool m_isRunning = false;

  private:
};

#endif // BOTHREAD_H
