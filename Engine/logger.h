#pragma once
#include <iostream>
#include <memory>
#include <string>

namespace engine
{
class Logger
{
public:
    static std::string GetTypeName(const char *typeid_name)
    {
        const auto str = std::string(typeid_name);
        auto n = str.find_last_of(':');
        if (n == std::string::npos)
        {
            Error("Could not find type name for %s", typeid_name);
            n = 5;
        }

        return str.substr(n + 1);
    }

    template <typename... Args>
    static std::unique_ptr<char[]> FormatString(const char *fmt, Args... args)
    {
        const auto size_s = std::snprintf(nullptr, 0, fmt, args...) + 1;
        if (size_s <= 0)
        {
            return nullptr;
        }
        const auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buffer(new char[size]);
        std::snprintf(buffer.get(), size, fmt, args...);
        return buffer;
    }

    template <typename... Args>
    static void Log(const char *level, const char *msg, Args... args)
    {
        std::cout << level << ": " << FormatString(msg, args...) << std::endl;
    }

    template <typename... Args>
    static void Log(const char *msg, Args... args)
    {
        Log("INFO", msg, args...);
    }

    template <typename T>
    static void Log(const char *msg)
    {
        Log("INFO", "[%s] %s", GetTypeName(typeid(T).name()).c_str(), msg);
    }

    template <typename T, typename... Args>
    static void Log(const char *msg, Args... args)
    {
        Log<T>(FormatString(msg, args...).get());
    }

    template <typename... Args>
    static void Error(const char *level, const char *msg, Args... args)
    {
        std::cerr << level << ": " << FormatString(msg, args...) << std::endl;
    }

    template <typename... Args>
    static void Error(const char *msg, Args... args)
    {
        Error("ERROR", msg, args...);
    }

    template <typename T>
    static void Error(const char *msg)
    {
        Error("ERROR", "[%s] %s", GetTypeName(typeid(T).name()).c_str(), msg);
    }

    template <typename T, typename... Args>
    static void Error(const char *msg, Args... args)
    {
        Error<T>(FormatString(msg, args...).get());
    }
};
}