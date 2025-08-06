#pragma once
#include "renderer.h"
#include "Components/camera.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class Renderer2D;

class Canvas : public Renderer
{
    Vector2 m_canvas_size_;
    AssetPtr<Camera> m_target_camera_;
    std::set<std::shared_ptr<Renderer2D>> m_children_renderers_;

public:
    void OnInspectorGui() override;
    void OnDraw() override;
    Vector2 CanvasSize();
    std::weak_ptr<Transform> BoundsOrigin() override;

    void AddRenderer(std::shared_ptr<Renderer2D> renderer);
    void RemoveRenderer(std::shared_ptr<Renderer2D> renderer);

};
}