#pragma once
#include "event.h"
#include "object.h"

namespace engine
{
class Camera;
class RenderPipeline
{
    friend Engine;
    friend Camera;
    std::unordered_set<std::shared_ptr<Camera>> m_cameras_;
    std::vector<std::weak_ptr<Renderer>> m_renderers_;

    void InvokeDrawCall();
    void Render(const std::shared_ptr<Camera> &camera);

public:
    Event<> on_rendering;

    static RenderPipeline *Instance();
    static void Init();
    static size_t GetRendererCount();

    static void AddCamera(std::shared_ptr<Camera> camera);
    static void RemoveCamera(const std::shared_ptr<Camera> &camera);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
};
}