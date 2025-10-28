#pragma once
#include <string>

namespace engine
{
struct ImportLog
{
    enum kLogType { kWarning, kError };

    std::string message;
    kLogType type;
};
}