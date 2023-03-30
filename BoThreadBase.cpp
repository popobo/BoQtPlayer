#include "BoThreadBase.h"
#include "BoLog.h"
#include <thread>

const int32_t STOP_WAIT_TIMES{200};
const int32_t PAUSE_WAIT_TIEMS{100};

void thread_sleep(int32_t ms) {
    std::chrono::milliseconds s_time(ms);
    std::this_thread::sleep_for(s_time);
}

void BoThreadBase::start_thread() {
    m_is_thread_exited = false;
    m_is_thread_running = false;
    m_is_thread_paused = false;
    std::thread th(&BoThreadBase::thread_main, this);
    th.detach();
}

void BoThreadBase::stop_thread() {
    m_is_thread_exited = true;
    for (int i = 0; i < STOP_WAIT_TIMES; ++i) {
        if (!m_is_thread_running) {
            BO_INFO("{0} thread stop successfully");
        }
        thread_sleep(1);
    }
    BO_INFO("{0} thread stop time out");
}

void BoThreadBase::try_pause_thread() {
    m_require_pause = true;
    for (int32_t i = 0; i < PAUSE_WAIT_TIEMS; ++i) {
        if (m_is_thread_paused == m_require_pause) {
            BO_INFO("{0} thread pause successfully");
            break;
        }
        thread_sleep(1);
    }
    BO_INFO("{0} thread pause time out");
}

void BoThreadBase::try_resume_thread() {
    m_require_pause = false;
    for (int32_t i = 0; i < PAUSE_WAIT_TIEMS; ++i) {
        if (m_is_thread_paused == m_require_pause) {
            BO_INFO("{0} thread resume successfully");
            break;
        }
        thread_sleep(1);
    }
    BO_INFO("{0} thread resume time out");
}

bool BoThreadBase::is_thread_paused()
{
    m_is_thread_paused = m_require_pause;
    return m_is_thread_paused;
}

void BoThreadBase::main_task() {}

void BoThreadBase::thread_main() {
    BO_INFO("{0} thread begin");
    m_is_thread_running = true;
}
