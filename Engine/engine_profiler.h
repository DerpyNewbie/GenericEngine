#pragma once
#include "../pch.h"

#include "engine_time.h"
#include "CaboRenderingSystem/RHI/gpu_profile.h"

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

    /// <summary>
    /// CRSから読み戻しが終わった最新のGPUプロファイルを取得し、gpu_profileを更新します。EndFrameの後に呼びます。
    /// </summary>
    static void UpdateGpuProfile();

    static bool m_gpu_profiling_enabled_;

public:
    static std::unordered_map<std::string, Instant> prev_begins;
    static std::unordered_map<std::string, Instant> prev_ends;
    static std::unordered_map<std::string, float> prev_durations;

    /// <summary>
    /// 最新のGPUプロファイルです。GPUの処理が終わってから読み戻すので、数フレーム前の結果になります。
    /// </summary>
    static CRS::GpuProfileResult gpu_profile;
    /// <summary>
    /// gpu_profileに有効な結果が入っているかどうかです。
    /// </summary>
    static bool has_gpu_profile;

    /// <summary>
    /// GPUプロファイラを有効 / 無効にします。有効にすると、パスごとのGPU時間と描画数を測ります。
    /// </summary>
    static void SetGpuProfilingEnabled(bool enabled);

    /// <summary>
    /// GPUプロファイラが有効かどうかです。
    /// </summary>
    static bool IsGpuProfilingEnabled()
    {
        return m_gpu_profiling_enabled_;
    }

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