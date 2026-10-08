#pragma once
#include "cinema_camera_component.h"

namespace engine
{

/// <summary>
/// 2つのCinemaCameraの間のBlendの状態です。
/// </summary>
struct BlendState
{
    std::shared_ptr<CinemaCameraComponent> from;
    std::shared_ptr<CinemaCameraComponent> to;
    float duration;
    float time;
};

/// <summary>
/// CinemaCameraの設定を実際のCameraに反映し、Camera間のBlendを行うComponentです。
/// </summary>
class CinemaBrainComponent : public Component
{
    AssetPtr<CameraComponent> m_target_camera_;
    bool m_is_blending_ = false;
    BlendState m_blend_;

    /// <summary>
    /// Blendの時間を進め、fromとtoのCinemaCameraの姿勢を補間した結果をTarget Cameraに適用します。時間を過ぎた場合はtoの姿勢に合わせてBlendを終了します。
    /// </summary>
    /// <param name="delta_time">進める時間(秒)</param>
    void DoBlending(float delta_time);

public:
    void OnInspectorGui() override;
    void OnUpdate() override;

    /// <summary>
    /// 2つのCinemaCameraの間のBlendを開始します。
    /// </summary>
    /// <param name="from">Blend元のCinemaCamera</param>
    /// <param name="to">Blend先のCinemaCamera</param>
    /// <param name="duration">Blendにかける時間(秒)</param>
    /// <param name="time">開始時点の経過時間(秒)</param>
    void Blend(const std::shared_ptr<CinemaCameraComponent> &from, const std::shared_ptr<CinemaCameraComponent> &to, float duration, float time = 0);

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_target_camera_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::CinemaBrainComponent, 1)