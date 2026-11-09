#include "pch.h"

#include "engine_profiler.h"

#include "Rendering/crs_bridge.h"
#include "CaboRenderingSystem/render_engine.h"

namespace engine
{
std::unordered_map<std::string, Instant> Profiler::m_begin_times_;
std::unordered_map<std::string, Instant> Profiler::m_end_times_;

std::unordered_map<std::string, Instant> Profiler::prev_begins;
std::unordered_map<std::string, Instant> Profiler::prev_ends;
std::unordered_map<std::string, float> Profiler::prev_durations;

bool Profiler::m_gpu_profiling_enabled_ = false;
CRS::GpuProfileResult Profiler::gpu_profile;
bool Profiler::has_gpu_profile = false;

void Profiler::NewFrame()
{
    const auto begin = Time::Instant();
    prev_begins.clear();
    prev_ends.clear();
    prev_durations.clear();

    const auto next_size = m_begin_times_.size();
    prev_begins.reserve(next_size);
    prev_ends.reserve(next_size);
    prev_durations.reserve(next_size + 1);

    for (const auto &[key, begin_time] : m_begin_times_)
    {
        auto end_time = m_end_times_[key];
        const auto duration = std::chrono::duration<float>(end_time - begin_time).count();

        prev_begins[key] = begin_time;
        prev_ends[key] = end_time;
        prev_durations[key] = duration;
    }

    m_begin_times_.clear();
    m_end_times_.clear();

    prev_durations.insert_or_assign("__ProfilerInternal",
                                    std::chrono::duration<float>(begin - Time::Instant()).count());
}

void Profiler::SetGpuProfilingEnabled(const bool enabled)
{
    m_gpu_profiling_enabled_ = enabled;
    CrsBridge::Crs().SetGpuProfilingEnabled(enabled);
    if (!enabled)
    {
        gpu_profile = {};
        has_gpu_profile = false;
    }
}

void Profiler::UpdateGpuProfile()
{
    if (!m_gpu_profiling_enabled_)
        return;

    CRS::GpuProfileResult result;
    if (CrsBridge::Crs().GetGpuProfile(result))
    {
        gpu_profile = std::move(result);
        has_gpu_profile = true;
    }
}
}
