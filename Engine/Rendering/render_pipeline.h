#pragma once
#include "object.h"

namespace engine
{
class Camera;
class RenderPipeline
{
    friend Engine;
    friend Camera;
    static std::unordered_set<std::shared_ptr<Camera>> m_cameras_;
    static std::vector<std::weak_ptr<Renderer>> m_renderers_;

    static void InvokeDrawCall();

public:
    static size_t GetRendererCount();

    static void SubScribeCamera(std::shared_ptr<Camera> camera);
    static void UnSubScribeCamera(const std::shared_ptr<Camera> &camera);
    static void SubscribeRenderer(std::shared_ptr<Renderer> renderer);
    static void UnSubscribeRenderer(const std::shared_ptr<Renderer> &renderer);
};
}