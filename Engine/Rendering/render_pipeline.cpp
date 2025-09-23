#include "pch.h"
#include "Components/camera_component.h"
#include "Components/renderer.h"
#include "render_pipeline.h"

#include "game_object.h"
#include "CabotEngine/Graphics/DescriptorHeap.h"
#include "gizmos.h"
#include "skybox.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/light.h"


namespace engine
{
void RenderPipeline::InvokeDrawCall()
{
    const auto cmd_list = RenderEngine::CommandList();

    for (const auto camera : m_lights_ | std::views::values)
    {
        camera->m_depth_texture_->BeginRender();
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

    if (const auto main_camera = CameraComponent::Main())
    {
        RenderEngine::Instance()->SetMainRenderTarget(main_camera->m_property_.background_color);
        cmd_list->SetGraphicsRootSignature(RootSignature::Get());
        const auto descriptor_heap = DescriptorHeap::GetHeap();
        cmd_list->SetDescriptorHeaps(1, &descriptor_heap);
        Render(main_camera);
        on_rendering.Invoke();
    }
}

void RenderPipeline::UpdateBuffer(const std::shared_ptr<CameraComponent> &camera)
{
    SetShadowMap();
    CameraComponent::SetCurrentCamera(camera);
    camera->SetViewProjMatrix();
    Skybox::Instance()->Render();
    Light::SetBuffers();
    if (m_is_updated_)
    {
        return;
    }
    for (const auto renderer : m_renderers_)
    {
        renderer->UpdateBuffer();
    }
}

void RenderPipeline::Render(const std::shared_ptr<CameraComponent> &camera)
{
    UpdateBuffer(camera);
    auto renderers = camera->FilterVisibleObjects(m_renderers_);
    for (auto renderer : renderers)
    {
        renderer->Render();
    }
    Gizmos::Render();
}

void RenderPipeline::DepthRender(const std::shared_ptr<CameraComponent> &camera)
{
    UpdateBuffer(camera);
    const auto renderers = camera->FilterVisibleObjects(m_renderers_);
    for (const auto renderer : renderers)
    {
        renderer->DepthRender();
    }
}

void RenderPipeline::SetShadowMap()
{
    if (m_depth_textures_ == nullptr)
    {
        m_depth_textures_ = std::make_shared<Texture2DArray>();
        D3D12_CLEAR_VALUE clear_value = {};
        clear_value.Format = DXGI_FORMAT_D32_FLOAT;
        clear_value.DepthStencil.Depth = 1.0f;
        clear_value.DepthStencil.Stencil = 0;
        m_depth_textures_->CreateResource(shadow_map_size, Light::kMaxLightCount, 1, DXGI_FORMAT_R32_TYPELESS,
                                          D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &clear_value);
        m_depth_textures_->SetFormat(DXGI_FORMAT_R32_FLOAT);
        m_shadowmap_handle_ = m_depth_textures_->UploadBuffer();
    }
    const auto cmd_list = RenderEngine::CommandList();
    if (!m_lights_.empty())
        cmd_list->SetGraphicsRootDescriptorTable(kShadowMapSRV,
                                                 m_shadowmap_handle_->HandleGPU);
}

RenderPipeline *RenderPipeline::Instance()
{
    static auto instance = new RenderPipeline;
    return instance;
}

size_t RenderPipeline::GetRendererCount()
{
    return Instance()->m_renderers_.size();
}

void RenderPipeline::AddLight(std::shared_ptr<Light> light)
{
    auto go = light->GameObject();
    auto camera = go->AddComponent<CameraComponent>();
    light->SetCamera(camera);
    camera->m_depth_texture_ = std::make_shared<DepthTexture>();
    camera->m_depth_texture_->SetResource(m_depth_textures_);
    m_lights_.emplace(light, camera);
}

void RenderPipeline::RemoveLight(const std::shared_ptr<Light> &light)
{
    auto depth_texture = m_lights_[light]->m_depth_texture_;
    m_depth_textures_->RemoveTexture(AssetPtr<Texture2D>::FromManaged(depth_texture));
    m_lights_.erase(light);
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

void RenderPipeline::AddCamera(std::shared_ptr<CameraComponent> camera)
{
    Instance()->m_cameras_.emplace(camera);
}

void RenderPipeline::RemoveCamera(const std::shared_ptr<CameraComponent> &camera)
{
    Instance()->m_cameras_.erase(camera);
}
}