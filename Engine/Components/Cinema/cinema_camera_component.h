#pragma once
#include "Components/component.h"
#include "Components/transform.h"
#include "Components/camera_component.h"
#include "Asset/asset_ptr.h"

namespace engine
{
/// <summary>
/// Cameraの設定と追従対象を持つ、仮想的なCameraのComponentです。
/// </summary>
class CinemaCameraComponent : public Component
{
    CameraProperty m_property_;

    bool m_apply_position_ = false;
    bool m_apply_rotation_ = false;

    AssetPtr<Transform> m_tracking_target_;
    Vector3 m_tracking_offset_ = {};

public:
    /// <summary>
    /// 追従対象からのOffsetを計算します。
    /// </summary>
    void OnAwake() override;
    void OnInspectorGui() override;

    /// <summary>
    /// 現在の位置から、追従対象のLocal空間でのOffsetを計算し直します。
    /// </summary>
    void RecalculateOffset();
    /// <summary>
    /// 設定に応じて、追従対象とOffsetから求めた位置と、追従対象の方を向く回転をTransformに適用します。
    /// </summary>
    void ApplyTransform() const;

    /// <summary>
    /// 追従対象のWorld空間での位置を取得します。追従対象がない場合は原点を返します。
    /// </summary>
    [[nodiscard]] Vector3 GetTargetPosition() const;
    /// <summary>
    /// 追従対象のWorld空間での回転を取得します。追従対象がない場合は回転なしを返します。
    /// </summary>
    [[nodiscard]] Quaternion GetTargetRotation() const;

    /// <summary>
    /// 自身のWorld空間での位置を取得します。
    /// </summary>
    [[nodiscard]] Vector3 GetPosition() const;
    /// <summary>
    /// 自身のWorld空間での回転を取得します。
    /// </summary>
    [[nodiscard]] Quaternion GetRotation() const;

    /// <summary>
    /// 自身の位置から追従対象の方を向く回転を取得します。
    /// </summary>
    [[nodiscard]] Quaternion GetLookAtRotation() const;
    /// <summary>
    /// 自身の位置から追従対象の方を向くMatrixを取得します。位置が同じ場合は単位行列を返します。
    /// </summary>
    [[nodiscard]] Matrix GetLookAtMatrix() const;

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