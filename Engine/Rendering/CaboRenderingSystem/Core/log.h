#pragma once
#include <cstdio>
#include <format>
#include <string>

namespace CRS
{
/// <summary>
/// CRS内部のログ出力です。エンジンのLoggerに依存しないよう、標準出力に同じ形式で書き出します。
/// </summary>
template <typename... Args>
void LogError(std::format_string<Args...> fmt, Args &&... args)
{
    const auto message = std::format(fmt, std::forward<Args>(args)...);
    std::printf("ERROR: [CRS] %s\n", message.c_str());
}

template <typename... Args>
void LogWarning(std::format_string<Args...> fmt, Args &&... args)
{
    const auto message = std::format(fmt, std::forward<Args>(args)...);
    std::printf("WARN: [CRS] %s\n", message.c_str());
}

template <typename... Args>
void LogInfo(std::format_string<Args...> fmt, Args &&... args)
{
    const auto message = std::format(fmt, std::forward<Args>(args)...);
    std::printf("INFO: [CRS] %s\n", message.c_str());
}
}