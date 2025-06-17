#pragma once
#include "engine_time.h"

#include <chrono>
#include <string>
#include <unordered_map>

namespace engine
{
class Profiler
{
    friend class Engine;

    static void NewFrame();

    static std::unordered_map<std::string, Instant> m_begin_times_;
    static std::unordered_map<std::string, Instant> m_end_times_;

public:
    static std::unordered_map<std::string, Instant> prev_begins;
    static std::unordered_map<std::string, Instant> prev_ends;
    static std::unordered_map<std::string, float> prev_durations;

    static void Begin(const std::string &name)
    {
        m_begin_times_.insert_or_assign(name, Time::Instant());
    }

    static void End(const std::string &name)
    {
        m_end_times_.insert_or_assign(name, Time::Instant());
    }

    template <typename T>
    static void Begin()
    {
        Begin(typeid(T).name());
    }

    template <typename T>
    static void End()
    {
        End(typeid(T).name());
    }
};
}