#pragma once
#include <string>

namespace engine
{
/// <summary>
/// Import中に出力されたログです。
/// </summary>
struct ImportLog
{
    /// <summary>
    /// ログの種類です。
    /// </summary>
    enum kLogType { kWarning, kError };

    std::string message;
    kLogType type;
};
}