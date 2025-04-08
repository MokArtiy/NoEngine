#pragma once

#include <spdlog/spdlog.h>

namespace NoEngine {

#ifdef NDEBUG

#define LOG_INFO(...)	spdlog::info(__VA_ARGS__)
#define LOG_WARN(...)	spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...)	spdlog::error(__VA_ARGS__)
#define LOG_CRITICAL(...)	spdlog::critical(__VA_ARGS__)

#else

#define LOG_INFO(...)	spdlog::info(__VA_ARGS__)
#define LOG_WARN(...)	spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...)	spdlog::error(__VA_ARGS__)
#define LOG_CRITICAL(...)	spdlog::critical(__VA_ARGS__)

#endif

}