#pragma once
#include "component.h"

namespace engine
{
/// <summary>
/// キー入力でTransformを移動、回転させるComponentです。
/// </summary>
class Controller : public Component
{
    float m_movement_speed_ = 1.0f;
    float m_rotation_speed_ = 2.0f;

    Vector3 m_rotation_ = {};

    Vector3 m_last_movement_input_;
    Vector2 m_last_rotation_input_;

public:
    /// <summary>
    /// キー入力(WASD、Space、左Ctrl、矢印キー)に応じてTransformを移動、回転させます。
    /// </summary>
    void OnUpdate() override;
    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_rotation_));
    }
};
}

CEREAL_CLASS_VERSION(engine::Controller, 1)