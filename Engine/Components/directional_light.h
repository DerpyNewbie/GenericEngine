#pragma once
#include "light.h"

namespace engine
{
class DirectionalLight : public Light
{
    float m_cover_size_ = 10.0f;

public:
    void OnInspectorGui() override;
    void OnUpdate() override;
};
}