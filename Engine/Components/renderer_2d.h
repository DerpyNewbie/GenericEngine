#pragma once
#include "canvas.h"
#include "Math/rect.h"

namespace engine
{
class Renderer2D : public Component
{
    AssetPtr<Canvas> m_canvas_;

public:
    void OnAwake() override;
    void OnUpdate() override;
    void OnDisabled() override;

    Rect NormalizedRect();

    virtual void Render() = 0;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_canvas_));
    }
};
}