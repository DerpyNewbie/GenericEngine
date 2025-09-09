#include "pch.h"
#include "Components/camera.h"
#include "Components/renderer.h"
#include "render_pipeline.h"
#include "Editor/editor.h"


namespace engine
{
std::unordered_set<std::shared_ptr<Camera>> RenderPipeline::m_cameras_;
std::vector<std::weak_ptr<Renderer>> RenderPipeline::m_renderers_;

void RenderPipeline::InvokeDrawCall()
{
    for (const auto camera : m_cameras_)
    {
        if (auto render_texture = camera->m_render_texture_.CastedLock())
        {
            camera->BeginRender();
            camera->Render();
        }
    }
    const auto main_camera = Camera::Main();
    RenderEngine::Instance()->SetMainRenderTarget(main_camera->m_property_.background_color);
    main_camera->Render();
    editor::Editor::Instance()->OnDraw();
}

size_t RenderPipeline::GetRendererCount()
{
    return m_renderers_.size();
}

void RenderPipeline::SubscribeRenderer(std::shared_ptr<Renderer> renderer)
{
    m_renderers_.emplace_back(renderer);
}

void RenderPipeline::UnSubscribeRenderer(const std::shared_ptr<Renderer> &renderer)
{
    const auto pos = std::ranges::find_if(m_renderers_, [&](const auto &r) {
        return r.lock() == renderer;
    });
    if (pos == m_renderers_.end())
        return;
    m_renderers_.erase(pos);
}

void RenderPipeline::SubScribeCamera(std::shared_ptr<Camera> camera)
{
    m_cameras_.emplace(camera);
}

void RenderPipeline::UnSubScribeCamera(const std::shared_ptr<Camera> &camera)
{
    m_cameras_.erase(camera);
}
}