#pragma once

#include <condition_variable>
#include <cstdint>
#include <string>

void thread_sleep(int32_t ms);

class BoThreadBase {
  public:
    BoThreadBase() = default;

    void start_thread();

    void stop_thread();

    void try_pause_thread();

    void try_resume_thread();

    bool is_thread_paused();

    virtual void main_task();

    // Considering the usage scenarios, other special member functions are not
    // defined
    virtual ~BoThreadBase() = default;

  protected:
    bool m_is_thread_exited{false};
    bool m_require_pause{false};
    bool m_is_thread_paused{false};
    bool m_is_thread_running{false};
    std::string m_thread_name{""};

  private:
    void thread_main();
};
