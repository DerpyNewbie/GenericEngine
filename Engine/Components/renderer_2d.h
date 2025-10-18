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
    void OnEnabled() override;
    void OnDisabled() override;

    void SetCanvas(const std::shared_ptr<Canvas> &canvas);
    Rect NormalizedRect();

    virtual void Render() = 0;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_canvas_));
    }
};
}