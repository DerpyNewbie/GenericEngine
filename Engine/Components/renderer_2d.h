#pragma once
#include "canvas.h"
#include "Math/rect.h"

namespace engine
{
class Renderer2D : public Component
{
    AssetPtr<Canvas> m_canvas_;

public:
    void OnUpdate() override;
    void OnDisabled() override;

    Rect NormalizedRect();

    virtual void Render() = 0;
};
}