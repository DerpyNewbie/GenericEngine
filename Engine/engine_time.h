//
// Created by derpy on 2024/08/26.
//
#pragma once

namespace engine
{
using Instant = std::chrono::time_point<std::chrono::steady_clock>;

/// <summary>
/// 経過時間、DeltaTime、フレーム数、FixedUpdateの間隔などの時間を管理するクラスです。
/// </summary>
class Time
{
    friend class Engine;
    /// <summary>
    /// 起動時刻を記録して各時刻を初期化し、Sceneが追加された時にTimeSinceLevelLoadをリセットするListenerを登録します。
    /// </summary>
    void Init();

    /// <summary>
    /// フレームを1つ進め、DeltaTime、FPS、各経過時間を更新します。
    /// </summary>
    void IncrementFrame();

    /// <summary>
    /// このフレームで実行すべきFixedUpdateの回数を計算します。
    /// </summary>
    /// <returns>FixedUpdateの回数</returns>
    int UpdateFixedFrameCount();

    /// <summary>
    /// 目標FPSに合わせて、次のフレームの時刻までSleepします。
    /// </summary>
    void WaitForNextFrame();

    Instant m_start_up_time_;
    Instant m_time_;
    Instant m_fps_check_time_;
    Instant m_fixed_update_check_time_;
    float m_fixed_delta_time_ = 0.02F;
    float m_delta_time_ = 0;
    float m_time_scale_ = 1;
    float m_time_since_start_up_ = 0;
    float m_unscaled_tim_since_start_up_ = 0;
    float m_time_since_level_load_ = 0;
    float m_unscaled_time_since_level_load_ = 0;
    int m_fps_ = 0;
    int m_fps_counter_ = 0;
    int m_last_fixed_frame_count_ = 0;
    int m_fps_target_ = 1000;

    double m_seconds_per_frame_ = 1.0 / m_fps_target_;
    unsigned int m_frames_ = 0;

    static Time *m_update_time_;

public:
    /// <summary>
    /// Timeの唯一のインスタンスを取得します。存在しない場合は生成します。
    /// </summary>
    [[nodiscard]] static Time *Get()
    {
        if (m_update_time_ == nullptr)
            m_update_time_ = new Time();
        return m_update_time_;
    }

    /// <summary>
    /// TimeScaleを掛けたDeltaTime(秒)を取得します。
    /// </summary>
    [[nodiscard]] static float GetDeltaTime()
    {
        return Get()->DeltaTime();
    }

    /// <summary>
    /// 現在のフレームの時刻をclockのtick数で取得します。
    /// </summary>
    [[nodiscard]] LONGLONG SourceTime() const
    {
        return m_time_.time_since_epoch().count();
    }

    /// <summary>
    /// FPSを最後に集計した時刻をclockのtick数で取得します。
    /// </summary>
    [[nodiscard]] LONGLONG LastCheckedSourceTime() const
    {
        return m_fps_check_time_.time_since_epoch().count();
    }

    /// <summary>
    /// FixedUpdateの間隔(秒)を取得します。
    /// </summary>
    [[nodiscard]] float FixedDeltaTime() const
    {
        return m_fixed_delta_time_;
    }

    /// <summary>
    /// 前フレームからの経過時間(秒)にTimeScaleを掛けた値を取得します。
    /// </summary>
    [[nodiscard]] float DeltaTime() const
    {
        return m_time_scale_ * m_delta_time_;
    }

    /// <summary>
    /// TimeScaleを掛けていない、前フレームからの経過時間(秒)を取得します。
    /// </summary>
    [[nodiscard]] const float &UnscaledDeltaTime() const
    {
        return m_delta_time_;
    }

    /// <summary>
    /// 起動してからの経過時間(秒)を取得します。TimeScaleの影響を受けます。
    /// </summary>
    [[nodiscard]] const float &TimeSinceStartUp() const
    {
        return m_time_since_start_up_;
    }

    /// <summary>
    /// 最後にSceneが追加されてからの経過時間(秒)を取得します。TimeScaleの影響を受けます。
    /// </summary>
    [[nodiscard]] const float &TimeSinceLevelLoad() const
    {
        return m_time_since_level_load_;
    }

    /// <summary>
    /// 直近1秒間のフレーム数を取得します。
    /// </summary>
    [[nodiscard]] const int &Fps() const
    {
        return m_fps_;
    }

    /// <summary>
    /// FPSの集計中の、現在のフレーム数のカウントを取得します。
    /// </summary>
    [[nodiscard]] const int &FpsCounter() const
    {
        return m_fps_counter_;
    }

    /// <summary>
    /// 目標FPSを取得します。
    /// </summary>
    [[nodiscard]] const int &FpsTarget() const
    {
        return m_fps_target_;
    }

    /// <summary>
    /// 目標FPSでの1フレームあたりの秒数を取得します。
    /// </summary>
    [[nodiscard]] const double &SecondsPerFrame() const
    {
        return m_seconds_per_frame_;
    }

    /// <summary>
    /// 直近のフレームで計算されたFixedUpdateの回数を取得します。
    /// </summary>
    [[nodiscard]] const int &LastFixedFrameCount() const
    {
        return m_last_fixed_frame_count_;
    }

    /// <summary>
    /// 起動してからの総フレーム数を取得します。
    /// </summary>
    [[nodiscard]] const unsigned int &Frames() const
    {
        return m_frames_;
    }

    /// <summary>
    /// TimeScaleを取得します。
    /// </summary>
    [[nodiscard]] const float &TimeScale() const
    {
        return m_time_scale_;
    }

    /// <summary>
    /// TimeScaleを設定します。
    /// </summary>
    /// <param name="time_scale">時間の進む倍率</param>
    void TimeScale(const float time_scale)
    {
        m_time_scale_ = time_scale;
    }

    /// <summary>
    /// 目標FPSを設定します。SecondsPerFrameも合わせて更新されます。
    /// </summary>
    /// <param name="target">目標FPS</param>
    void FpsTarget(const int target)
    {
        m_fps_target_ = target;
        m_seconds_per_frame_ = 1.0 / m_fps_target_;
    }

    /// <summary>
    /// FixedUpdateの間隔(秒)を設定します。
    /// </summary>
    /// <param name="fixed_delta_time">FixedUpdateの間隔(秒)</param>
    void FixedDeltaTime(const float fixed_delta_time)
    {
        m_fixed_delta_time_ = fixed_delta_time;
    }

    /// <summary>
    /// 現在のフレームが始まってからの経過時間(秒)を取得します。秒未満は切り捨てられます。
    /// </summary>
    [[nodiscard]] double CurrentFrameTime() const;

    /// <summary>
    /// 現在の時刻を取得します。
    /// </summary>
    static Instant Instant();
};
}