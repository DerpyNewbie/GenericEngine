#pragma once
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
    std::list<std::function<void()>> m_draw_calls_;

    void InvokeDrawCall();
    void Render(const std::shared_ptr<Camera> &camera);

public:
    static RenderPipeline *Instance();
    static void Init();
    static size_t GetRendererCount();

    static void AddDrawCall(std::function<void()> draw_call);
    static void AddCamera(std::shared_ptr<Camera> camera);
    static void RemoveCamera(const std::shared_ptr<Camera> &camera);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
};
}