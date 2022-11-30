#ifndef BOLOG_H
#define BOLOG_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
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

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define BO_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(BoLog::getLogger(), __VA_ARGS__)
#define BO_ERROR(...) SPDLOG_LOGGER_ERROR(BoLog::getLogger(), __VA_ARGS__)
#define BO_WARN(...) SPDLOG_LOGGER_WARN(BoLog::getLogger(), __VA_ARGS__)
#define BO_INFO(...) SPDLOG_LOGGER_INFO(BoLog::getLogger(), __VA_ARGS__)
#define BO_TRACE(...) SPDLOG_LOGGER_TRACE(BoLog::getLogger(), __VA_ARGS__)
#else
#define BO_CRITICAL(...) BoLog::getLogger()->critical(__VA_ARGS__)
#define BO_ERROR(...) BoLog::getLogger()->error(__VA_ARGS__)
#define BO_WARN(...) BoLog::getLogger()->warn(__VA_ARGS__)
#define BO_INFO(...) BoLog::getLogger()->info(__VA_ARGS__)
#define BO_TRACE(...) BoLog::getLogger()->trace(__VA_ARGS__)
#endif

#endif // BOLOG_H
