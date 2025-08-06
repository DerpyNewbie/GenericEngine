#pragma once
#include "canvas.h"
#include "Math/rect.h"

namespace engine
{
class Renderer2D : public Component
{

public:
    AssetPtr<Canvas> canvas;

    void OnEnabled() override;
    void OnDisabled() override;

    Rect NormalizedRect();

    virtual void Render() = 0;
};
}