#pragma once
#include "component.h"
#include "Math/color.h"

namespace engine
{
class Camera : public Component
{
    enum kViewMode
    {
        kPerspective,
        kOrthographic
    };

    static std::weak_ptr<Camera> m_main_camera_;

    kViewMode m_view_mode_ = kPerspective;
    float m_field_of_view_ = 70;
    float m_near_plane_ = 0.1f;
    float m_far_plane_ = 1000.0f;
    float m_ortho_size_ = 50;
    Color m_background_color_ = Color(0x1A1A1AFF);

    void ApplyCameraSettingToDxLib() const;

public:
    void OnAwake() override;
    void OnUpdate() override;

    static std::weak_ptr<Camera> Main();

    Matrix GetViewMatrix() const;
    Matrix GetProjectionMatrix() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_view_mode_),
           CEREAL_NVP(m_field_of_view_),
           CEREAL_NVP(m_ortho_size_),
           CEREAL_NVP(m_near_plane_),
           CEREAL_NVP(m_far_plane_),
           CEREAL_NVP(m_background_color_));
    }
};
}