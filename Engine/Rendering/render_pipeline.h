#pragma once
#include "event.h"
#include "object.h"
#include "CabotEngine/Graphics/Texture2DArray.h"
#include "Components/directional_light.h"
#include "Components/light.h"

namespace engine
{
class Camera;
class RenderPipeline
{
    friend Engine;
    friend Camera;
    static constexpr Vector2 shadow_map_size = {1920, 1065};
    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::unordered_set<std::shared_ptr<Camera>> m_cameras_;
    std::unordered_map<std::shared_ptr<Light>, std::shared_ptr<Camera>> m_lights_;
    std::shared_ptr<Texture2DArray> m_depth_textures_;
    std::shared_ptr<DescriptorHandle> m_shadowmap_handle_;
    bool m_is_updated_ = false;

    void InvokeDrawCall();
    void UpdateBuffer(const std::shared_ptr<Camera> &camera);
    void Render(const std::shared_ptr<Camera> &camera);
    void DepthRender(const std::shared_ptr<Camera> &camera);

    void SetShadowMap();

public:
    Event<> on_rendering;

    static RenderPipeline *Instance();
    static size_t GetRendererCount();

    void AddLight(std::shared_ptr<Light> light);
    void RemoveLight(const std::shared_ptr<Light> &light);
    static void AddCamera(std::shared_ptr<Camera> camera);
    static void RemoveCamera(const std::shared_ptr<Camera> &camera);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
};
}