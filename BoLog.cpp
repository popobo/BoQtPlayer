#include "BoLog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> BoLog::s_logger{nullptr};

void BoLog::init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_logger = spdlog::stdout_color_mt("BoPlayer");
    s_logger->set_level(spdlog::level::trace);
}
