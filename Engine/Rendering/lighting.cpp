#include "pch.h"
#include "game_object.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/camera_component.h"
#include "lighting.h"

namespace
{
std::array<Vector3, 8> CalcFrustumCorners(const Matrix &cam_view, const Matrix &cam_proj)
{
    const Vector3 ndc_corners[8] =
    {
        {-1, -1, 0}, {-1, 1, 0}, {1, 1, 0}, {1, -1, 0},
        {-1, -1, 1}, {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}
    };

    const Matrix inv_view_proj = (cam_view * cam_proj).Invert();
    std::array<Vector3, 8> corners;
    for (int i = 0; i < 8; i++)
        corners[i] = Vector3::Transform(ndc_corners[i], inv_view_proj);

    return corners;
}
}

namespace engine
{
void Lighting::CreateShadowMapResource()
{
    m_depth_textures_ = std::make_shared<Texture2DArray>();
    D3D12_CLEAR_VALUE clear_value;
    clear_value.Format = DXGI_FORMAT_D32_FLOAT;
    clear_value.DepthStencil.Depth = 1.0f;
    clear_value.DepthStencil.Stencil = 0;
    m_depth_textures_->CreateResource(RenderingConstants::kShadowMapSize, RenderingConstants::kMaxShadowMapCount, 1,
                                      DXGI_FORMAT_R32_TYPELESS,
                                      D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &clear_value);
    m_depth_textures_->SetFormat(DXGI_FORMAT_R32_FLOAT);
    m_shadow_map_handle_ = m_depth_textures_->UploadBuffer();

    m_free_depth_texture_handles_.clear();
    for (UINT i = 0; i < RenderingConstants::kMaxShadowMapCount; i++)
        m_free_depth_texture_handles_.emplace(i);

    auto device = RenderEngine::Device();
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsv_heap_));
    if (FAILED(hr))
    {
        Logger::Error<DepthTexture>("Failed To Create DSV Heap for DepthTexture");
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
    dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    dsv_desc.Texture2DArray.ArraySize = RenderingConstants::kMaxShadowMapCount;
    dsv_desc.Texture2DArray.FirstArraySlice = 0;
    dsv_desc.Texture2DArray.MipSlice = 0;

    device->CreateDepthStencilView(m_depth_textures_->Resource(), &dsv_desc,
                                   Instance()->m_dsv_heap_->GetCPUDescriptorHandleForHeapStart());
}

Lighting *Lighting::Instance()
{
    static auto instance = new Lighting;
    return instance;
}

void Lighting::SetCurrentShadowMapIndex(int shadow_map_index)
{
    if (m_current_shadow_map_index_buffer_[0] == nullptr)
    {
        for (int i = 0; i < m_current_shadow_map_index_buffer_.size(); ++i)
        {
            const auto cb = m_current_shadow_map_index_buffer_[i] = std::make_shared<ConstantBuffer>(sizeof(int));
            cb->CreateBuffer();
            cb->UpdateBuffer(&i);
        }
    }

    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootConstantBufferView(kLightCountCBV,
                                                m_current_shadow_map_index_buffer_[shadow_map_index]->GetAddress());
}

void Lighting::UpdateLightsViewProjMatrixBuffer()
{
    int light_vp_idx = 0;
    for (const auto light : m_lights_)
    {
        if (light->m_depth_texture_handle_.empty())
            continue;

        const auto shadow_map_count = light->ShadowMapCount();
        const auto camera = CameraComponent::Current();
        const auto camera_property = camera->m_property_;

        auto frustum_corners = CalcFrustumCorners(camera->ViewMatrix(), camera->m_property_.ProjectionMatrix());
        auto light_vp = light->CalcViewProj(frustum_corners);

        for (int i = 0; i < shadow_map_count; ++i)
        {
            m_light_view_proj_matrices_[++light_vp_idx] = light_vp[i];
        }
    }

    while (light_vp_idx < RenderingConstants::kMaxShadowMapCount)
    {
        m_light_view_proj_matrices_[++light_vp_idx] = Matrix::Identity;
    }

    if (m_light_view_proj_matrices_buffer_ == nullptr)
    {
        m_light_view_proj_matrices_buffer_ = std::make_shared<StructuredBuffer>(
            sizeof(Matrix), RenderingConstants::kMaxShadowMapCount);
        m_light_view_proj_matrices_buffer_->CreateBuffer();
        m_light_view_proj_handle_ = m_light_view_proj_matrices_buffer_->UploadBuffer();
    }

    m_light_view_proj_matrices_buffer_->UpdateBuffer(m_light_view_proj_matrices_.data());
}

void Lighting::SetCascadeSlicesBuffer()
{
    if (m_cascade_slices_buffer_ == nullptr)
    {
        m_cascade_slices_buffer_ = std::make_shared<ConstantBuffer>(
            sizeof(float) * RenderingConstants::kShadowCascadeCount);
        m_cascade_slices_buffer_->CreateBuffer();
    }

    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootConstantBufferView(kCascadeSlicesCBV, m_cascade_slices_buffer_->GetAddress());
}

void Lighting::SetLightsViewProjMatrix() const
{
    const auto cmd_list = RenderEngine::CommandList();

    cmd_list->SetGraphicsRootDescriptorTable(kLightViewProj, m_light_view_proj_handle_->HandleGPU);
}

void Lighting::SetShadowMap()
{
    if (m_depth_textures_ == nullptr)
    {
        CreateShadowMapResource();
    }
    const auto cmd_list = RenderEngine::CommandList();
    if (!m_lights_.empty())
        cmd_list->SetGraphicsRootDescriptorTable(kShadowMapSRV,
                                                 m_shadow_map_handle_->HandleGPU);
}

void Lighting::TryApplyShadow(const std::shared_ptr<Light> &light)
{
    const auto go = light->GameObject();
    const auto shadow_map_count = light->ShadowMapCount();

    if (m_free_depth_texture_handles_.size() < shadow_map_count)
    {
        light->m_light_data_.cast_shadow = false;
        m_waiting_lights_.emplace_back(light);
        return;
    }

    for (int i = 0; i < shadow_map_count; ++i)
    {
        auto handle_it = m_free_depth_texture_handles_.begin();

        const auto handle = *handle_it;
        m_free_depth_texture_handles_.erase(handle_it);
        light->m_depth_texture_handle_.emplace_back(handle);

        auto depth_texture = std::make_shared<DepthTexture>();
        depth_texture->SetResource(m_depth_textures_, handle);
        if (m_shadow_maps_.size() <= handle)
            m_shadow_maps_.emplace_back(depth_texture);
        else
            m_shadow_maps_[handle] = depth_texture;
    }
    light->m_light_data_.cast_shadow = true;
}

void Lighting::RemoveShadow(const std::shared_ptr<Light> &light)
{
    if (light->m_depth_texture_handle_.empty())
    {
        erase_if(m_waiting_lights_, [&](const std::shared_ptr<Light> &a) {
            return a == light;
        });
        return;
    }

    const auto shadow_map_count = light->ShadowMapCount();
    for (int i = 0; i < shadow_map_count; ++i)
    {
        m_depth_textures_->RemoveTexture(
            AssetPtr<Texture2D>::FromManaged(m_shadow_maps_[light->m_depth_texture_handle_[i]]));

        m_shadow_maps_[light->m_depth_texture_handle_[i]].reset();
        m_free_depth_texture_handles_.emplace(light->m_depth_texture_handle_[i]);
    }
    light->m_depth_texture_handle_.clear();

    if (!m_waiting_lights_.empty())
    {
        auto camera_pos = CameraComponent::Main()->GameObject()->Transform()->Position();
        auto it = std::ranges::min_element(m_waiting_lights_,
                                           [camera_pos](const std::shared_ptr<Light> &a,
                                                        const std::shared_ptr<Light> &b) {
                                               return (camera_pos - a->GetPos()).Length() < (camera_pos - b->GetPos()).
                                                      Length();
                                           });
        TryApplyShadow(*it);
        m_waiting_lights_.erase(it);
    }
}

void Lighting::AddLight(std::shared_ptr<Light> light)
{
    m_lights_.emplace_back(light);
}

void Lighting::RemoveLight(const std::shared_ptr<Light> &light)
{
    std::erase_if(m_lights_, [&](const std::shared_ptr<Light> &this_light) {
        return light == this_light;
    });
    if (light->m_depth_texture_handle_.empty())
        return;

    RemoveShadow(light);
}

void Lighting::SetCascadeSlices(
    std::array<float, RenderingConstants::kShadowCascadeCount> shadow_cascade_slices)
{
    auto &cascade_slices_buffer = Instance()->m_cascade_slices_buffer_;
    if (cascade_slices_buffer == nullptr)
    {
        cascade_slices_buffer = std::make_shared<ConstantBuffer>(
            sizeof(float) * RenderingConstants::kShadowCascadeCount);
        cascade_slices_buffer->CreateBuffer();
    }

    cascade_slices_buffer->UpdateBuffer(shadow_cascade_slices.data());
}
}