#pragma once
#include "renderer.h"
#include "Components/camera_component.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class Renderer2D;

class Canvas : public Renderer
{
    Vector2 m_canvas_size_;
    AssetPtr<CameraComponent> m_target_camera_;
    std::list<std::shared_ptr<Renderer2D>> m_children_renderers_;

public:
    void OnInspectorGui() override;
    void OnAwake() override;
    void Render() override;
    Vector2 CanvasSize();
    std::shared_ptr<Transform> BoundsOrigin() override;

    void AddRenderer(const std::shared_ptr<Renderer2D> &renderer);
    void RemoveRenderer(const std::shared_ptr<Renderer2D> &renderer);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_canvas_size_), CEREAL_NVP(m_target_camera_));
    }
};
}