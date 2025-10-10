#include "pch.h"
#include "render_pipeline.h"
#include "Components/camera_component.h"
#include "Components/renderer.h"
#include "gizmos.h"
#include "rendering_settings_component.h"
#include "skybox.h"
#include "view_projection.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/light.h"

using namespace DirectX;

namespace
{
std::array<Vector3, 8> CalcFrustumCorners(const Matrix &cam_view, const Matrix &cam_proj)
{
    std::array<Vector3, 8> corners;

    Vector3 ndc_corners[8] =
    {
        {-1, -1, 0}, {-1, 1, 0}, {1, 1, 0}, {1, -1, 0},
        {-1, -1, 1}, {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}
    };

    Matrix inv_view_proj = (cam_view * cam_proj).Invert();

    std::array<Vector3, 8> fullFrustum;
    for (int i = 0; i < 8; i++)
    {
        Vector3 corner = XMVectorSet(ndc_corners[i].x, ndc_corners[i].y, ndc_corners[i].z, 1.0f);
        corner = XMVector3TransformCoord(corner, inv_view_proj);
        fullFrustum[i] = corner;
    }

    return fullFrustum;
}

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

    for (const auto camera : m_cameras_)
    {
        CameraComponent::SetCurrentCamera(camera);
        UpdateLightsViewProjMatrixBuffer();

        DepthRender();

        ID3D12DescriptorHeap *rtv_heap = nullptr;
        ID3D12DescriptorHeap *dsv_heap = nullptr;
        auto render_tex = camera->m_render_texture_.CastedLock();
        if (render_tex)
        {
            render_tex->BeginRender(camera->m_property_.background_color);
            rtv_heap = render_tex->GetHeap();
        }

        if (auto depth_texture = camera->m_depth_texture_.CastedLock())
        {
            depth_texture->BeginRender();
            dsv_heap = depth_texture->GetHeap();
        }

        if (rtv_heap != nullptr || dsv_heap != nullptr)
        {
            RenderEngine::Instance()->SetRenderTarget(rtv_heap, dsv_heap, camera->m_property_.background_color);

            auto view = camera->ViewMatrix();
            auto &property = camera->m_property_;
            Render(view, property);
        }

        if (render_tex)
            render_tex->EndRender();

        if (const auto depth_texture = camera->m_depth_texture_.CastedLock())
            depth_texture->EndRender();
    }

    if (const auto main_camera = CameraComponent::Main())
    {
        CameraComponent::SetCurrentCamera(main_camera);
        UpdateLightsViewProjMatrixBuffer();

        DepthRender();

        RenderEngine::Instance()->SetMainRenderTarget(main_camera->m_property_.background_color);
        const auto view = main_camera->ViewMatrix();
        const auto &property = main_camera->m_property_;
        Render(view, property);
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
    SetLightsViewProjMatrix();
    SetShadowMap();
    SetCascadeSlicesBuffer();
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

void RenderPipeline::Render(const Matrix &view, const CameraProperty &camera_property)
{
    const auto proj = camera_property.ProjectionMatrix();
    UpdateBuffer(view, proj);
    auto renderers = FilterVisibleObjects(m_renderers_, view, proj);
    for (auto renderer : renderers)
    {
        renderer->Render();
    }
    Gizmos::Render();
}

void RenderPipeline::DepthRender()
{
    auto current_shadow_map_count = 0;
    for (const auto light : m_lights_)
    {
        const auto cmd_list = RenderEngine::CommandList();
        cmd_list->SetPipelineState(PSOManager::Get("Depth"));

        const auto shadow_map_count_in_light = light->ShadowMapCount();

        for (int i = 0; i < shadow_map_count_in_light; ++i)
        {
            m_shadow_maps_[current_shadow_map_count]->BeginRender();
            RenderEngine::Instance()->SetRenderTarget(nullptr, m_shadow_maps_[i]->GetHeap(),
                                                      Color());

            SetViewProjMatrix(m_light_view_proj_matrices_[i], Matrix::Identity);
            SetLightsViewProjMatrix();
            SetCurrentShadowMapIndex(i);
            if (!m_is_updated_)
            {
                for (const auto renderer : m_renderers_)
                {
                    renderer->UpdateBuffer();
                }
            }
            for (const auto renderer : m_renderers_)
            {
                renderer->DepthRender();
            }
            m_shadow_maps_[i]->EndRender();
            ++current_shadow_map_count;
        }
    }
}

void RenderPipeline::UpdateLightsViewProjMatrixBuffer()
{
    int current_matrices_index = 0;
    for (const auto light : m_lights_)
    {
        const auto shadow_map_count = light->ShadowMapCount();
        const auto camera = CameraComponent::Current();
        const auto camera_property = camera->m_property_;

        auto frustum_corners = CalcFrustumCorners(camera->ViewMatrix(), camera->m_property_.ProjectionMatrix());
        auto matrices = light->CalcViewProj(frustum_corners);

        for (int i = 0; i < shadow_map_count; ++i)
        {
            m_light_view_proj_matrices_[current_matrices_index] = matrices[i];
            ++current_matrices_index;
        }
    }

    if (m_light_view_proj_matrices_buffer_ == nullptr)
    {
        m_light_view_proj_matrices_buffer_ = std::make_shared<StructuredBuffer>(
            sizeof(Matrix), kMaxShadowMapCount);
        m_light_view_proj_matrices_buffer_->CreateBuffer();
        m_light_view_proj_handle_ = m_light_view_proj_matrices_buffer_->UploadBuffer();
    }

    m_light_view_proj_matrices_buffer_->UpdateBuffer(m_light_view_proj_matrices_.data());
}

void RenderPipeline::SetCurrentShadowMapIndex(int shadow_map_index)
{
    if (m_current_shadow_map_index_buffer_[0] == nullptr)
    {
        for (int i = 0; i < m_current_shadow_map_index_buffer_.size(); ++i)
        {
            m_current_shadow_map_index_buffer_[i] = std::make_shared<ConstantBuffer>(sizeof(int));
            m_current_shadow_map_index_buffer_[i]->CreateBuffer();
            m_current_shadow_map_index_buffer_[i]->UpdateBuffer(&i);
        }
    }

    auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootConstantBufferView(kLightCountCBV,
                                                m_current_shadow_map_index_buffer_[shadow_map_index]->GetAddress());
}

void RenderPipeline::SetCascadeSlices(
    std::array<float, RenderingSettingsComponent::kShadowCascadeCount> shadow_cascade_sprits)
{
    Instance()->m_cascade_slices_buffer_->UpdateBuffer(shadow_cascade_sprits.data());
}

void RenderPipeline::SetCascadeSlicesBuffer()
{
    if (m_cascade_slices_buffer_ == nullptr)
    {
        m_cascade_slices_buffer_ = std::make_shared<ConstantBuffer>(
            sizeof(float) * RenderingSettingsComponent::kShadowCascadeCount);
        m_cascade_slices_buffer_->CreateBuffer();
    }

    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootConstantBufferView(kCascadeSlicesCBV, m_cascade_slices_buffer_->GetAddress());
}

void RenderPipeline::SetLightsViewProjMatrix() const
{
    const auto cmd_list = RenderEngine::CommandList();

    cmd_list->SetGraphicsRootDescriptorTable(kLightViewProj, m_light_view_proj_handle_->HandleGPU);
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
        m_depth_textures_->CreateResource(kShadowMapSize, kMaxShadowMapCount, 1,
                                          DXGI_FORMAT_R32_TYPELESS,
                                          D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &clear_value);
        m_depth_textures_->SetFormat(DXGI_FORMAT_R32_FLOAT);
        m_shadow_map_handle_ = m_depth_textures_->UploadBuffer();

        m_free_depth_texture_handles_.clear();
        for (int i = 0; i < kMaxShadowMapCount; i++)
        {
            m_free_depth_texture_handles_.emplace(i);
        }
    }
    const auto cmd_list = RenderEngine::CommandList();
    if (!m_lights_.empty())
        cmd_list->SetGraphicsRootDescriptorTable(kShadowMapSRV,
                                                 m_shadow_map_handle_->HandleGPU);
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
    const auto go = light->GameObject();
    auto shadow_map_count = light->ShadowMapCount();

    for (int si = 0; si < shadow_map_count; ++si)
    {
        std::shared_ptr<DepthTexture> depth_texture;
        depth_texture = std::make_shared<DepthTexture>();

        auto i = m_free_depth_texture_handles_.begin();
        if (i == m_free_depth_texture_handles_.end())
        {
            Logger::Error<RenderPipeline>("Could not retrieve depth texture: Pool is empty");
            return;
        }

        const auto index = *i;
        m_free_depth_texture_handles_.erase(i);

        light->m_depth_texture_handle_.emplace_back(index);
        depth_texture->SetResource(m_depth_textures_, index);
        m_shadow_maps_.emplace_back(depth_texture);
    }
    m_lights_.emplace_back(light);

}

void RenderPipeline::RemoveLight(const std::shared_ptr<Light> &light)
{
    std::erase_if(m_lights_, [light](const std::shared_ptr<Light> &this_light) {
        return light == this_light;
    });

    const auto shadowmap_count = light->ShadowMapCount();
    for (int si = 0; shadowmap_count < si; ++si)
    {
        m_depth_textures_->RemoveTexture(AssetPtr<Texture2D>::FromManaged(m_shadow_maps_[si]));

        m_shadow_maps_[si].reset();
        m_free_depth_texture_handles_.emplace(light->m_depth_texture_handle_[si]);
    }
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