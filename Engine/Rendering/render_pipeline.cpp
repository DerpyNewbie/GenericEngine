#include "pch.h"
#include "render_pipeline.h"
#include "Components/camera_component.h"
#include "Components/renderer.h"
#include "gizmos.h"
#include "lighting.h"
#include "skybox.h"
#include "view_projection.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/light.h"

using namespace DirectX;

namespace
{
std::vector<std::shared_ptr<engine::Renderer>> FilterVisibleObjects(
    const std::vector<std::shared_ptr<engine::Renderer>> &renderers, const Matrix &view, const Matrix &proj)
{
    BoundingFrustum frustum;
    BoundingFrustum::CreateFromMatrix(frustum, proj, true);
    frustum.Transform(frustum, view.Invert());

    std::vector<std::shared_ptr<engine::Renderer>> results;
    for (auto renderer : renderers)
    {
        auto world_matrix = renderer->BoundsOrigin()->WorldMatrix();

        BoundingBox world_bounds;
        renderer->bounds.Transform(world_bounds, world_matrix);

        if (frustum.Intersects(world_bounds))
        {
            results.emplace_back(renderer);
        }
    }
    return results;
}
}

namespace engine
{
void RenderPipeline::InvokeDrawCall()
{
    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootSignature(RootSignature::Get());
    const auto descriptor_heap = DescriptorHeap::GetHeap();
    cmd_list->SetDescriptorHeaps(1, &descriptor_heap);

    for (const auto renderer : m_renderers_)
    {
        renderer->UpdateBuffer();
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

        auto depth_tex = camera->m_depth_texture_.CastedLock();
        if (depth_tex)
        {
            depth_tex->BeginRender();
            dsv_heap = depth_tex->GetHeap();
        }

        if (rtv_heap == nullptr && dsv_heap == nullptr)
            continue;

        CameraComponent::SetCurrentCamera(camera);
        Lighting::Instance()->UpdateLightsViewProjMatrixBuffer();

        DepthRender();

        RenderEngine::Instance()->SetRenderTarget(rtv_heap, dsv_heap, camera->m_property_.background_color);
        Render(camera);

        if (render_tex)
            render_tex->EndRender();

        if (depth_tex)
            depth_tex->EndRender();
    }

    if (const auto main_camera = CameraComponent::Main())
    {
        CameraComponent::SetCurrentCamera(main_camera);
        Lighting::Instance()->UpdateLightsViewProjMatrixBuffer();
        DepthRender();

        RenderEngine::Instance()->SetMainRenderTarget(main_camera->m_property_.background_color);
        Render(main_camera);
        on_rendering.Invoke();
    }
}

void RenderPipeline::SetViewProjMatrix(const Matrix &view, const Matrix &proj)
{
    if (m_view_proj_matrix_buffers_[0] == nullptr)
    {
        for (auto &view_proj_matrix_buffer : m_view_proj_matrix_buffers_)
        {
            view_proj_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(ViewProjection));
            view_proj_matrix_buffer->CreateBuffer();
        }
    }

    const auto cmd_list = RenderEngine::CommandList();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto view_projection_buffer = m_view_proj_matrix_buffers_[current_buffer_idx];
    ViewProjection view_projection;
    view_projection.matrices[0] = view;
    view_projection.matrices[1] = proj;
    view_projection_buffer->UpdateBuffer(&view_projection);

    cmd_list->SetGraphicsRootConstantBufferView(kViewProjCBV, view_projection_buffer->GetAddress());
}

void RenderPipeline::UpdateBuffer(const Matrix &view, const Matrix &proj)
{
    SetViewProjMatrix(view, proj);
    auto lighting_instance = Lighting::Instance();
    lighting_instance->SetLightsViewProjMatrix();
    lighting_instance->SetShadowMap();
    lighting_instance->SetCascadeSlicesBuffer();
    Skybox::Instance()->Render();
    Light::SetBuffers();

    for (const auto renderer : m_renderers_)
    {
        renderer->UpdateBuffer();
    }
}

void RenderPipeline::Render(const std::shared_ptr<CameraComponent> &camera)
{
    const auto view = camera->ViewMatrix();
    const auto proj = camera->m_property_.ProjectionMatrix();
    UpdateBuffer(view, proj);

    const auto renderers = FilterVisibleObjects(m_renderers_, view, proj);
    for (const auto renderer : renderers)
    {
        renderer->Render();
    }

    Gizmos::Render();
}

void RenderPipeline::DepthRender()
{
    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetPipelineState(PSOManager::Get("Depth"));

    for (int i = 0; i < Lighting::Instance()->m_lights_.size(); ++i)
    {
        for (auto itr : Lighting::Instance()->m_lights_[i]->m_depth_texture_handle_)
        {
            Lighting::Instance()->m_shadow_maps_[itr]->BeginRender();
        }
    }
    RenderEngine::Instance()->SetRenderTarget(nullptr, Lighting::Instance()->m_dsv_heap_.Get(),
                                              Color());

    auto lighting_instance = Lighting::Instance();
    lighting_instance->SetLightsViewProjMatrix();

    for (const auto renderer : m_renderers_)
    {
        renderer->DepthRender();
    }

    for (int i = 0; i < Lighting::Instance()->m_lights_.size(); ++i)
    {
        for (auto itr : Lighting::Instance()->m_lights_[i]->m_depth_texture_handle_)
        {
            Lighting::Instance()->m_shadow_maps_[itr]->EndRender();
        }
    }
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

void RenderPipeline::AddRenderer(std::shared_ptr<Renderer> renderer)
{
    Instance()->m_renderers_.emplace_back(renderer);
}

void RenderPipeline::RemoveRenderer(const std::shared_ptr<Renderer> &renderer)
{
    auto &renderers = Instance()->m_renderers_;
    std::erase_if(renderers, [&](const auto &r) {
        return r == renderer;
    });
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