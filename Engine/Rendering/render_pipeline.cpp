#include "pch.h"
#include "camera.h"
#include "Components/renderer.h"
#include "render_pipeline.h"
#include "gizmos.h"
#include "Components/light.h"
#include "Components/skybox.h"


namespace engine
{
void RenderPipeline::InvokeDrawCall()
{
    for (const auto camera : m_cameras_)
    {
        ID3D12DescriptorHeap *rtv_heap = nullptr;
        ID3D12DescriptorHeap *dsv_heap = nullptr;
        auto render_tex = camera->m_render_texture_.CastedLock();
        if (render_tex)
        {
            render_tex->BeginRender(camera->m_property_.background_color);
            rtv_heap = render_tex->GetHeap();
        }

        if (camera->m_depth_texture_)
        {
            camera->m_depth_texture_->BeginRender();
            dsv_heap = camera->m_depth_texture_->GetHeap();
        }

        if (rtv_heap != nullptr || dsv_heap != nullptr)
        {
            RenderEngine::Instance()->SetRenderTarget(rtv_heap, dsv_heap, camera->m_property_.background_color);
            Render(camera);
        }

        if (render_tex)
        {
            render_tex->EndRender();
        }

        if (camera->m_depth_texture_)
        {
            camera->m_depth_texture_->EndRender();
        }
    }

    if (const auto main_camera = Camera::Main())
    {
        RenderEngine::Instance()->SetMainRenderTarget(main_camera->m_property_.background_color);
        Render(main_camera);
        on_rendering.Invoke();
    }
}

void RenderPipeline::Render(const std::shared_ptr<Camera> &camera)
{
    camera->SetViewProjMatrix();
    Light::SetLightBuffers();
    auto renderers = camera->FilterVisibleObjects(m_renderers_);
    for (auto renderer : renderers)
    {
        renderer->OnDraw();
    }
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