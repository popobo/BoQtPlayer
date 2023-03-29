#include "ElapsedTimer.h"

ElapsedTimer::ElapsedTimer() { m_prevTime = Clock::now(); }

int64_t ElapsedTimer::elapsed() {
    ClockTimePoint currentTime = Clock::now();
    auto diffTime = currentTime - m_prevTime;
    m_prevTime = currentTime;

    return std::chrono::duration_cast<std::chrono::milliseconds>(diffTime)
        .count();
}
