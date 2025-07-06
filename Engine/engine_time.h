//
// Created by derpy on 2024/08/26.
//
#pragma once

namespace engine
{
using Instant = std::chrono::time_point<std::chrono::steady_clock>;

class Time
{
    friend class Engine;
    void Init();

    void IncrementFrame();

    int UpdateFixedFrameCount();

    void WaitForNextFrame();

    Instant m_start_up_time_;
    Instant m_time_;
    Instant m_fps_check_time_;
    Instant m_fixed_update_check_time_;
    float m_dt_ = 0;
    float m_time_scale_ = 1;
    float m_time_since_start_up_ = 0;
    int m_fps_ = 0;
    int m_fps_counter_ = 0;
    int m_last_fixed_frame_count_ = 0;
    int m_fps_target_ = 1000;

    double m_seconds_per_frame_ = 1.0 / m_fps_target_;
    unsigned int m_frames_ = 0;

    static Time *m_update_time_;

public:
    [[nodiscard]] static Time *Get()
    {
        if (m_update_time_ == nullptr)
            m_update_time_ = new Time();
        return m_update_time_;
    }

    [[nodiscard]] static float GetDeltaTime()
    {
        return Get()->DeltaTime();
    }

    [[nodiscard]] LONGLONG SourceTime() const
    {
        return m_time_.time_since_epoch().count();
    }

    [[nodiscard]] LONGLONG LastCheckedSourceTime() const
    {
        return m_fps_check_time_.time_since_epoch().count();
    }

    [[nodiscard]] float DeltaTime() const
    {
        return m_time_scale_ * m_dt_;
    }

    [[nodiscard]] const float &UnscaledDeltaTime() const
    {
        return m_dt_;
    }

    [[nodiscard]] const float &TimeSinceStartUp() const
    {
        return m_time_since_start_up_;
    }

    [[nodiscard]] const int &Fps() const
    {
        return m_fps_;
    }

    [[nodiscard]] const int &FpsCounter() const
    {
        return m_fps_counter_;
    }

    [[nodiscard]] const int &FpsTarget() const
    {
        return m_fps_target_;
    }

    [[nodiscard]] const double &SecondsPerFrame() const
    {
        return m_seconds_per_frame_;
    }

    [[nodiscard]] const int &LastFixedFrameCount() const
    {
        return m_last_fixed_frame_count_;
    }

    [[nodiscard]] const unsigned int &Frames() const
    {
        return m_frames_;
    }

    [[nodiscard]] const float &TimeScale() const
    {
        return m_time_scale_;
    }

    void TimeScale(const float time_scale)
    {
        m_time_scale_ = time_scale;
    }

    void FpsTarget(const int target)
    {
        m_fps_target_ = target;
        m_seconds_per_frame_ = 1.0 / m_fps_target_;
    }

    [[nodiscard]] double CurrentFrameTime() const;

    static Instant Instant();
};
}