#pragma once
#include "color.h"
#include "component.h"

namespace engine
{
class Camera : public Component
{
    enum kViewMode
    {
        kPerspective,
        kOrthographic
    };

    kViewMode m_view_mode_ = kPerspective;
    float m_field_of_view_ = 70;
    float m_near_plane_ = 0.1f;
    float m_far_plane_ = 1000.0f;
    float m_ortho_size_ = 50;
    Color m_background_color_ = Color(0x1A1A1AFF);

    void ApplyCameraSettingToDxLib() const;

public:
    void OnAwake() override
    {}

    void OnStart() override
    {}
    
    void OnUpdate() override;

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