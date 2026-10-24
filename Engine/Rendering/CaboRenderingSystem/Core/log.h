#pragma once
#include <cstdio>
#include <format>
#include <string>

namespace CRS
{
/// <summary>
/// エラーをログに出します。CRSはエンジンのLoggerに依存しないよう、標準出力にエンジンと同じ形式で書き出します。
/// </summary>
template <typename... Args>
void LogError(std::format_string<Args...> fmt, Args &&... args)
{
    const auto message = std::format(fmt, std::forward<Args>(args)...);
    std::printf("ERROR: [CRS] %s\n", message.c_str());
}

/// <summary>
/// 警告をログに出します（LogErrorと同じ形式）。
/// </summary>
template <typename... Args>
void LogWarning(std::format_string<Args...> fmt, Args &&... args)
{
    const auto message = std::format(fmt, std::forward<Args>(args)...);
    std::printf("WARN: [CRS] %s\n", message.c_str());
}

/// <summary>
/// 情報をログに出します（LogErrorと同じ形式）。
/// </summary>
template <typename... Args>
void LogInfo(std::format_string<Args...> fmt, Args &&... args)
{
    const auto message = std::format(fmt, std::forward<Args>(args)...);
    std::printf("INFO: [CRS] %s\n", message.c_str());
}
}
