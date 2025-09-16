#pragma once
#include "event.h"
#include "object.h"
#include "Components/directional_light.h"
#include "Components/light.h"

namespace engine
{
class Camera;
class RenderPipeline
{
    friend Engine;
    friend Camera;
    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::unordered_set<std::shared_ptr<Camera>> m_cameras_;
    std::unordered_map<std::shared_ptr<Light>, std::shared_ptr<Camera>> m_lights_;

    void InvokeDrawCall();
    void Render(const std::shared_ptr<Camera> &camera) const;
    void DepthRender(const std::shared_ptr<Camera> &camera) const;

public:
    Event<> on_rendering;

    static RenderPipeline *Instance();
    static void Init();
    static size_t GetRendererCount();

    static void AddLight(std::shared_ptr<Light> light);
    static void RemoveLight(const std::shared_ptr<Light> &light);
    static void AddCamera(std::shared_ptr<Camera> camera);
    static void RemoveCamera(const std::shared_ptr<Camera> &camera);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
};
}