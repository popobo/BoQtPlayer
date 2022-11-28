#ifndef BOLOG_H
#define BOLOG_H

#include "spdlog/spdlog.h"

class BoLog {
  public:
    static void init();

    inline static std::shared_ptr<spdlog::logger> &getLogger() {
        return s_logger;
    }

  private:
    static std::shared_ptr<spdlog::logger> s_logger;
};

#define BO_CRITICAL(...) BoLog::getLogger()->critical(__VA_ARGS__)
#define BO_ERROR(...) BoLog::getLogger()->error(__VA_ARGS__)
#define BO_WARN(...) BoLog::getLogger()->warn(__VA_ARGS__)
#define BO_INFO(...) BoLog::getLogger()->info(__VA_ARGS__)
#define BO_TRACE(...) BoLog::getLogger()->trace(__VA_ARGS__)

#endif // BOLOG_H
