//
// Created by derpy on 2024/08/26.
//

#include "engine_time.h"

#include <chrono>
#include <thread>

namespace engine
{
using namespace std::chrono;
Time *Time::m_update_time_;

void Time::IncrementFrame()
{
    const auto now = Instant();
    m_dt_ = duration<float>(now - m_time_).count();
    m_time_ = now;

    ++m_fps_counter_;
    if (duration<float>(now - m_fps_check_time_).count() >= 1.0)
    {
        m_fps_ = m_fps_counter_;
        m_fps_counter_ = 0;
        m_fps_check_time_ = now;
    }

    m_time_since_start_up_ = duration<float>(now - m_start_up_time_).count();
    ++m_frames_;
}

int Time::UpdateFixedFrameCount()
{
    int frame_counts = 0;
    while (m_fixed_update_check_time_ < m_time_)
    {
        m_fixed_update_check_time_ += duration_cast<steady_clock::duration>(duration<double>(0.02));
        frame_counts++;
    }

    m_last_fixed_frame_count_ = frame_counts;
    return frame_counts;
}
void Time::WaitForNextFrame()
{
    const auto ft = CurrentFrameTime();
    if (SecondsPerFrame() > ft)
    {
        std::this_thread::sleep_for(duration_cast<seconds>(duration<double>(SecondsPerFrame() - ft)));
    }
}

void Time::Init()
{
    m_start_up_time_ = Instant();
    m_time_ = m_start_up_time_;
    m_fps_check_time_ = m_start_up_time_;
    m_fixed_update_check_time_ = m_start_up_time_;
}

double Time::CurrentFrameTime() const
{
    return duration<double>(duration_cast<seconds>(Instant() - m_time_)).count();
}
Instant Time::Instant()
{
    return high_resolution_clock::now();
}
}