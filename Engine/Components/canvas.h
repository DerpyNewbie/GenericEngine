#pragma once
#include "Components/renderer.h"
#include "Components/camera_component.h"

namespace engine
{
class Renderer2D;

struct RendererComparator
{
    bool operator()(const std::shared_ptr<Renderer2D> &a, const std::shared_ptr<Renderer2D> &b) const;
};
class Canvas : public Renderer
{
    Vector2 m_canvas_size_;
    AssetPtr<CameraComponent> m_target_camera_;
    std::set<std::shared_ptr<Renderer2D>, RendererComparator> m_children_renderers_;

public:
    void OnInspectorGui() override;
    void OnAwake() override;
    void OnStart() override;
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