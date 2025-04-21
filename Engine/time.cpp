//
// Created by derpy on 2024/08/26.
//

#include "time.h"

namespace engine
{
Time* Time::m_update_time_;

void Time::IncrementFrame() {
    const LONGLONG now = GetNowHiPerformanceCount();
    m_dt_ = (now - m_time_) / 1000000.0f;
    m_time_ = now;

    ++m_fps_counter_;
    if (now - m_fps_check_time_ > 1000000.0f) {
        m_fps_ = m_fps_counter_;
        m_fps_counter_ = 0;
        m_fps_check_time_ = now;
    }

    m_time_since_start_up_ += m_dt_;
    ++m_frames_;
}

int Time::UpdateFixedFrameCount() {
    int frame_counts = 0;
    while (m_fixed_update_check_time_ < m_time_) {
        m_fixed_update_check_time_ += 20000;
        frame_counts++;
    }

    m_last_fixed_frame_count_ = frame_counts;
    return frame_counts;
}
void Time::WaitForNextFrame()
{
    auto ft = CurrentFrameTime();
    if (SecondsPerFrame() > ft) {
        WaitTimer(static_cast<int>(SecondsPerFrame() - ft) * 1000);
    }
}

void Time::Init() {
    m_time_ = GetNowHiPerformanceCount();
    m_fps_check_time_ = GetNowHiPerformanceCount();
    m_fixed_update_check_time_ = GetNowHiPerformanceCount();
}

double Time::CurrentFrameTime() const {
    const LONGLONG now = GetNowHiPerformanceCount();
    return (now - m_time_) / 1000000.0;
}
}