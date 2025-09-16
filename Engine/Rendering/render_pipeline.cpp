#include "pch.h"
#include "camera.h"
#include "Components/renderer.h"
#include "render_pipeline.h"

#include "game_object.h"
#include "CabotEngine/Graphics/DescriptorHeap.h"
#include "gizmos.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/light.h"
#include "Components/skybox.h"


namespace engine
{
void RenderPipeline::InvokeDrawCall()
{
    const auto cmd_list = RenderEngine::CommandList();

    for (const auto camera : m_lights_ | std::views::values)
    {
        camera->m_depth_texture_->BeginRender();
        camera->SetViewProjMatrix();
        RenderEngine::Instance()->SetRenderTarget(nullptr, camera->m_depth_texture_->GetHeap(),
                                                  camera->m_property_.background_color);
        cmd_list->SetGraphicsRootSignature(RootSignature::Get());
        auto descriptor_heap = DescriptorHeap::GetHeap();
        cmd_list->SetDescriptorHeaps(1, &descriptor_heap);
        DepthRender(camera);
        camera->m_depth_texture_->EndRender();
    }

    for (const auto camera : m_cameras_)
    {
        ID3D12DescriptorHeap *rtv_heap = nullptr;
        ID3D12DescriptorHeap *dsv_heap = nullptr;
        auto render_tex = camera->m_render_texture_.CastedLock();
        if (render_tex)
        {
            render_tex->BeginRender(camera->m_property_.background_color);
        }

        if (camera->m_depth_texture_)
        {
            camera->m_depth_texture_->BeginRender();
            dsv_heap = camera->m_depth_texture_->GetHeap();
        }

        if (rtv_heap != nullptr || dsv_heap != nullptr)
        {
            RenderEngine::Instance()->SetRenderTarget(rtv_heap, dsv_heap, camera->m_property_.background_color);
            cmd_list->SetGraphicsRootSignature(RootSignature::Get());
            auto descriptor_heap = DescriptorHeap::GetHeap();

            cmd_list->SetDescriptorHeaps(1, &descriptor_heap);
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
        cmd_list->SetGraphicsRootSignature(RootSignature::Get());
        const auto descriptor_heap = DescriptorHeap::GetHeap();
        cmd_list->SetDescriptorHeaps(1, &descriptor_heap);
        Render(main_camera);
        on_rendering.Invoke();
    }
}

void RenderPipeline::Render(const std::shared_ptr<Camera> &camera) const
{
    camera->SetViewProjMatrix();
    Light::SetBuffers();
    auto renderers = camera->FilterVisibleObjects(m_renderers_);
    for (auto renderer : renderers)
    {
        renderer->UpdateBuffer();
        renderer->Render();
    }
    Gizmos::Render();
}

void RenderPipeline::DepthRender(const std::shared_ptr<Camera> &camera) const
{
    camera->SetViewProjMatrix();
    Light::SetBuffers();
    const auto renderers = camera->FilterVisibleObjects(m_renderers_);
    for (const auto renderer : renderers)
    {
        renderer->UpdateBuffer();
        renderer->DepthRender();
    }
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

void RenderPipeline::AddLight(std::shared_ptr<Light> light)
{
    auto camera = std::make_shared<Camera>();
    camera->SetTransform(light->GameObject()->Transform());
    Instance()->m_lights_.emplace(light, camera);
}

void RenderPipeline::RemoveLight(const std::shared_ptr<Light> &light)
{
    Instance()->m_lights_.erase(light);
}

void RenderPipeline::AddRenderer(std::shared_ptr<Renderer> renderer)
{
    Instance()->m_renderers_.emplace_back(renderer);
}

void RenderPipeline::RemoveRenderer(const std::shared_ptr<Renderer> &renderer)
{
    auto &renderers = Instance()->m_renderers_;
    const auto pos = std::ranges::find_if(renderers, [&](const auto &r) {
        return r == renderer;
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