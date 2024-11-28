#pragma once

#include <iostream>
#include <sstream>
#include <format>

namespace hl
{
namespace utils
{
namespace logging
{

// 日志级别枚举
enum LogLevel
{
    LOG_LEVEL_FATAL = 1,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE
};

// 编译期颜色获取
const consteval char* get_color(LogLevel level)
{
    switch (level)
    {
        case LOG_LEVEL_FATAL   : return "\033[31m";    // 红色
        case LOG_LEVEL_ERROR   : return "\033[33m";    // 黄色
        case LOG_LEVEL_WARNING : return "\033[34m";    // 蓝色
        case LOG_LEVEL_INFO    : return "\033[32m";    // 绿色
        case LOG_LEVEL_DEBUG   : return "\033[36m";    // 青色
        case LOG_LEVEL_VERBOSE : return "\033[37m";    // 白色
        default                : return "\033[0m";                    // 默认颜色
    }
}

// 编译期枚举转字符串
const consteval char* to_string(LogLevel level)
{
    switch (level)
    {
        case LOG_LEVEL_FATAL   : return "FATAL";
        case LOG_LEVEL_ERROR   : return "ERROR";
        case LOG_LEVEL_WARNING : return "WARNING";
        case LOG_LEVEL_INFO    : return "INFO";
        case LOG_LEVEL_DEBUG   : return "DEBUG";
        case LOG_LEVEL_VERBOSE : return "VERBOSE";
        default                : return "UNKNOWN";
    }
}

// 编译期定义日志级别
template <LogLevel level>
void log_message(std::string_view tag, std::string_view msg)
{
    const constexpr char* color     = get_color(level);
    const constexpr char* level_str = to_string(level);
    const constexpr char* reset     = "\033[0m";
    std::cerr << std::format("{}[{}] {}: {}{}", color, tag, level_str, msg, reset) << std::endl;
}

// 宏定义来简化调用
#define HL_LOG_FATAL(tag, msg)   hl::utils::logging::log_message<hl::utils::logging::LOG_LEVEL_FATAL>(tag, msg)
#define HL_LOG_ERROR(tag, msg)   hl::utils::logging::log_message<hl::utils::logging::LOG_LEVEL_ERROR>(tag, msg)
#define HL_LOG_WARNING(tag, msg) hl::utils::logging::log_message<hl::utils::logging::LOG_LEVEL_WARNING>(tag, msg)
#define HL_LOG_INFO(tag, msg)    hl::utils::logging::log_message<hl::utils::logging::LOG_LEVEL_INFO>(tag, msg)
#define HL_LOG_DEBUG(tag, msg)   hl::utils::logging::log_message<hl::utils::logging::LOG_LEVEL_DEBUG>(tag, msg)
#define HL_LOG_VERBOSE(tag, msg) hl::utils::logging::log_message<hl::utils::logging::LOG_LEVEL_VERBOSE>(tag, msg)

}    // namespace logging
}    // namespace utils
}    // namespace hl
