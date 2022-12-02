#pragma once

#include <chrono>

class ElapsedTimer {
  public:
    using Clock = std::chrono::steady_clock;
    using ClockTimePoint = Clock::time_point;

    ElapsedTimer();

    int elapsed();

  private:
    ClockTimePoint m_prevTime;
};
