#pragma once
#include "../pch.h"

#include "engine_time.h"

namespace engine
{
/// <summary>
/// 名前ごとに処理時間を計測するProfilerです。
/// </summary>
class Profiler
{
    friend class Engine;

    /// <summary>
    /// 前フレームの計測結果をprev_begins / prev_ends / prev_durationsに移し、計測中のデータをクリアします。
    /// </summary>
    static void NewFrame();

    static std::unordered_map<std::string, Instant> m_begin_times_;
    static std::unordered_map<std::string, Instant> m_end_times_;

public:
    static std::unordered_map<std::string, Instant> prev_begins;
    static std::unordered_map<std::string, Instant> prev_ends;
    static std::unordered_map<std::string, float> prev_durations;

    /// <summary>
    /// 指定された名前の計測を開始します。
    /// </summary>
    /// <param name="name">計測の名前</param>
    static void Begin(const std::string &name)
    {
        m_begin_times_.insert_or_assign(name, Time::Instant());
    }

    /// <summary>
    /// 指定された名前の計測を終了します。
    /// </summary>
    /// <param name="name">計測の名前</param>
    static void End(const std::string &name)
    {
        m_end_times_.insert_or_assign(name, Time::Instant());
    }

    /// <summary>
    /// T のクラス名を名前として計測を開始します。
    /// </summary>
    template <typename T>
    static void Begin()
    {
        Begin(typeid(T).name());
    }

    /// <summary>
    /// T のクラス名を名前として計測を終了します。
    /// </summary>
    template <typename T>
    static void End()
    {
        End(typeid(T).name());
    }
};
}