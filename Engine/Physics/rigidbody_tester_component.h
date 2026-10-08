#pragma once
#include "Components/component.h"

namespace engine
{
/// <summary>
/// RigidbodyTesterComponentのテストの種類です。
/// </summary>
enum class kTestType : unsigned char
{
    kStarting,
    kForce,
    kImpulse,
    kForceAtPosition
};

/// <summary>
/// Rigidbodyの動作確認用のComponentです。
/// </summary>
class RigidbodyTesterComponent : public Component
{
    kTestType m_test_type_ = kTestType::kStarting;
    Vector3 m_force_ = {10, 0, 0};
    Vector3 m_position_ = {0, 0, 0};

public:
    /// <summary>
    /// TestTypeに応じて、Rigidbodyを初期状態に戻す、または力を加え続けます。Rigidbodyがない場合は追加します。
    /// </summary>
    void OnFixedUpdate() override;
    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_test_type_),
            CEREAL_NVP(m_force_),
            CEREAL_NVP(m_position_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::RigidbodyTesterComponent, 1)