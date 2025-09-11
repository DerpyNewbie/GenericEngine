#include "pch.h"
#include "Components/camera.h"
#include "Components/renderer.h"
#include "render_pipeline.h"
#include "gizmos.h"
#include "Components/skybox.h"


namespace engine
{
void RenderPipeline::InvokeDrawCall()
{
    for (const auto camera : m_cameras_)
    {
        if (auto render_texture = camera->m_render_texture_.CastedLock())
        {
            render_texture->BeginRender(camera->m_property_.background_color);
            Render(camera);
        }
    }
    const auto main_camera = Camera::Main();
    RenderEngine::Instance()->SetMainRenderTarget(main_camera->m_property_.background_color);
    Render(main_camera);
    for (auto draw_call : m_draw_calls_)
        draw_call();
}

void RenderPipeline::Render(const std::shared_ptr<Camera> &camera)
{
    Camera::SetCurrentCamera(camera);
    camera->SetViewProjMatrix();
    Skybox::Render();

    const auto objects_in_view = camera->FilterVisibleObjects(m_renderers_);
    for (const auto object : objects_in_view)
        object->OnDraw();
    Gizmos::Render();
}

RenderPipeline *RenderPipeline::Instance()
{
    static auto instance = new RenderPipeline;
    return instance;
}

void RenderPipeline::Init()
{
    Skybox::Initialize();
}

size_t RenderPipeline::GetRendererCount()
{
    return Instance()->m_renderers_.size();
}

void RenderPipeline::AddDrawCall(std::function<void()> draw_call)
{
    Instance()->m_draw_calls_.emplace_back(draw_call);
}

void RenderPipeline::AddRenderer(std::shared_ptr<Renderer> renderer)
{
    Instance()->m_renderers_.emplace_back(renderer);
}

void RenderPipeline::RemoveRenderer(const std::shared_ptr<Renderer> &renderer)
{
    auto &renderers = Instance()->m_renderers_;
    const auto pos = std::ranges::find_if(renderers, [&](const auto &r) {
        return r.lock() == renderer;
    });
    if (pos == renderers.end())
        return;
    renderers.erase(pos);
}

void RenderPipeline::AddCamera(std::shared_ptr<Camera> camera)
{
    Instance()->m_cameras_.emplace(camera);
}

void RenderPipeline::RemoveCamera(const std::shared_ptr<Camera> &camera)
{
    Instance()->m_cameras_.erase(camera);
}
}