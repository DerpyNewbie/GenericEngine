#pragma once
#include "Components/component.h"
#include "Components/transform.h"
#include "Components/camera_component.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class CinemaCameraComponent : public Component
{
    CameraProperty m_property_;

    bool m_apply_position_ = false;
    bool m_apply_rotation_ = false;

    AssetPtr<Transform> m_tracking_target_;
    Vector3 m_tracking_offset_ = {};

public:
    void OnAwake() override;
    void OnInspectorGui() override;

    void RecalculateOffset();
    void ApplyTransform();

    [[nodiscard]] Vector3 GetTargetPosition();
    [[nodiscard]] Quaternion GetTargetRotation();

    [[nodiscard]] Vector3 GetPosition() const;
    [[nodiscard]] Quaternion GetRotation() const;

    [[nodiscard]] Quaternion GetLookAtRotation();
    [[nodiscard]] Matrix GetLookAtMatrix();

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_property_),
            CEREAL_NVP(m_apply_position_),
            CEREAL_NVP(m_apply_rotation_),
            CEREAL_NVP(m_tracking_target_),
            CEREAL_NVP(m_tracking_offset_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::CinemaCameraComponent, 1)