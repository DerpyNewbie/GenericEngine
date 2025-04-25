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
    float m_field_of_view_ = 103;
    float m_ortho_size_ = 50;
    Color m_background_color_ = Color(0xFF000000);

    void ApplyCameraSettingToDxLib();

public:
    void OnUpdate() override;
};
}