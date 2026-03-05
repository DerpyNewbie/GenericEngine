#include "pch.h"
#include "raytracing_manager.h"

#include "raytracing_global_root_signature.h"
#include "raytracing_pipeline_state.h"
#include "Components/camera_component.h"
#include "Rendering/render_pipeline.h"

void engine::RaytracingManager::Submit(const std::shared_ptr<MeshRenderer> &mesh_renderer)
{
    auto mesh = mesh_renderer->GetSharedMesh().CastedLock();
    auto materials = mesh_renderer->shared_materials;
    auto matrix = mesh_renderer->GameObject()->Transform()->WorldMatrix();

    mesh->CreateBlts();

    D3D12_RAYTRACING_INSTANCE_DESC raytrace_instance_desc = {};

    raytrace_instance_desc.InstanceID = m_generic_material_datas_.size();
    raytrace_instance_desc.InstanceMask = 0xFF;
    raytrace_instance_desc.InstanceContributionToHitGroupIndex = 0;
    raytrace_instance_desc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
    raytrace_instance_desc.AccelerationStructure = mesh->blts->GetGPUVirtualAddress();

    DirectX::XMFLOAT3X4 dest_matrix;
    XMStoreFloat3x4(&dest_matrix, matrix);

    memcpy(raytrace_instance_desc.Transform, &dest_matrix, sizeof(DirectX::XMFLOAT3X4));

    m_tlas_instances_.emplace_back(raytrace_instance_desc);
    m_generic_material_datas_.emplace_back(materials[0].CastedLock()->p_shared_material_block->generic_material_data);

    ByteAddressBuffer vertex_buffer(mesh->vertex_buffer->Resource());
    ByteAddressBuffer index_buffer(mesh->index_buffers[0]->Resource());
    m_vertex_address_buffers_.emplace_back(vertex_buffer);
    m_index_address_buffers_.emplace_back(index_buffer);
}

void engine::RaytracingManager::UpdateBuffers()
{
    for (int i = 0; i < m_mesh_renderers_.size(); ++i)
    {
        auto matrix = m_mesh_renderers_[i]->GameObject()->Transform()->WorldMatrix();
        UpdateTransform(i, matrix);

        auto materials = m_mesh_renderers_[i]->shared_materials;
        UpdateMaterial(i, materials);
    }

    UpdateVertexIndexBuffer();
    m_material_buffer_->UpdateBuffer(m_generic_material_datas_.data());
    m_instance_info_buffer_->UpdateBuffer(m_instance_infos_.data());

    m_tlas_->Update(m_tlas_instances_);
}

void engine::RaytracingManager::UpdateVertexIndexBuffer()
{
    for (int i = 0; i < m_vertex_address_buffers_.size(); ++i)
    {
        auto vert_desc_handle = RenderPipeline::GetDynamicDescriptorHeap()->Allocate();
        auto index_desc_handle = RenderPipeline::GetDynamicDescriptorHeap()->Allocate();

        m_vertex_address_buffers_[i].UploadBuffer(vert_desc_handle);
        m_index_address_buffers_[i].UploadBuffer(index_desc_handle);

        m_instance_infos_.emplace_back(vert_desc_handle.index, index_desc_handle.index);

        m_vertex_buffer_handle_.emplace_back(vert_desc_handle);
        m_index_buffer_handle_.emplace_back(index_desc_handle);
    }
}

void engine::RaytracingManager::UpdateTransform(const int index, const Matrix &matrix)
{
    DirectX::XMFLOAT3X4 dest_matrix;
    XMStoreFloat3x4(&dest_matrix, matrix);

    memcpy(m_tlas_instances_[index].Transform, &dest_matrix, sizeof(DirectX::XMFLOAT3X4));
}
void engine::RaytracingManager::UpdateMaterial(int index, std::vector<AssetPtr<Material>> &materials)
{
    m_generic_material_datas_[index] = materials[0].CastedLock()->p_shared_material_block->generic_material_data;
}

engine::RaytracingManager *engine::RaytracingManager::Instance()
{
    static auto instance = new RaytracingManager;
    return instance;
}

void engine::RaytracingManager::Init()
{
    const auto instance = Instance();
    instance->m_raytracing_shaders_.emplace_back(std::make_shared<RaytracingShader>(L"Resources/Raytracing.raytrace"));
    RaytracingPipelineState::Instance()->CreateDxrPipelineState(*instance->m_raytracing_shaders_[0].get());
    instance->m_uav_textures_.emplace_back(Object::Instantiate<UavTexture>("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
    instance->m_uav_textures_[0]->CreateBuffer();
    instance->m_shader_table_ = std::make_shared<ShaderTable>();
    instance->m_tlas_ = std::make_shared<TopLevelAccelerationStructure>();
    instance->m_uav_texture_handle_ = RenderPipeline::GetStaticDescriptorHeap()->Allocate();
    instance->m_uav_textures_[0]->UploadBuffer(instance->m_uav_texture_handle_);

    for (auto &view_proj_buffer : instance->m_view_proj_buffers_)
    {
        view_proj_buffer = std::make_shared<ConstantBuffer>(sizeof(ViewProjection));
        view_proj_buffer->CreateBuffer();
    }

    instance->m_material_buffer_ = std::make_shared<StructuredBuffer>(sizeof(GenericMaterialData), 10);
    instance->m_material_buffer_->CreateBuffer();

    instance->m_instance_info_buffer_ = std::make_shared<StructuredBuffer>(sizeof(InstanceInfo), 10);
    instance->m_instance_info_buffer_->CreateBuffer();
}

void engine::RaytracingManager::Execute()
{
    for (auto it = m_no_mesh_mesh_renderers_.begin(); it != m_no_mesh_mesh_renderers_.end();)
    {
        auto &no_mesh_mesh_renderer = *it;
        auto mesh = no_mesh_mesh_renderer->GetSharedMesh().CastedLock();
        if (mesh == nullptr)
        {
            ++it;
            continue;
        }

        Submit(no_mesh_mesh_renderer);
        m_mesh_renderers_.emplace_back(no_mesh_mesh_renderer);
        it = m_no_mesh_mesh_renderers_.erase(it);
    }

    if (m_mesh_renderers_.empty())
        return;

    UpdateBuffers();

    m_uav_textures_[0]->BeginRender();

    auto dxr_command_list = RenderEngine::DxrCommandList();
    auto current_back_buffer_index = RenderEngine::CurrentBackBufferIndex();

    const auto main_camera = CameraComponent::Main();
    const auto view = main_camera->ViewMatrix().Invert();
    const auto proj = main_camera->property.ProjectionMatrix().Invert();

    const auto view_proj_buffer = m_view_proj_buffers_[current_back_buffer_index];
    ViewProjection view_projection;
    view_projection.matrices[0] = view;
    view_projection.matrices[1] = proj;
    view_proj_buffer->UpdateBuffer(&view_projection);

    dxr_command_list->SetPipelineState1(RaytracingPipelineState::Get().Get());
    dxr_command_list->SetComputeRootSignature(RaytracingGlobalRootSignature::Get());
    dxr_command_list->SetComputeRootConstantBufferView(0, view_proj_buffer->GetAddress());
    dxr_command_list->SetComputeRootDescriptorTable(1, m_uav_texture_handle_.HandleGPU);
    dxr_command_list->SetComputeRootShaderResourceView(2, m_tlas_->GetGPUVirtualAddress());
    dxr_command_list->SetComputeRootShaderResourceView(3, m_material_buffer_->GetAddress());
    dxr_command_list->SetComputeRootShaderResourceView(4, m_instance_info_buffer_->GetAddress());
    dxr_command_list->SetComputeRootDescriptorTable(5, m_vertex_buffer_handle_[0].HandleGPU);

    D3D12_DISPATCH_RAYS_DESC dispatch_desc = {};

    auto ray_gen_shader = m_shader_table_->RayGenShader();
    dispatch_desc.RayGenerationShaderRecord.StartAddress = ray_gen_shader->GetGPUVirtualAddress();
    dispatch_desc.RayGenerationShaderRecord.SizeInBytes = ray_gen_shader->GetDesc().Width;

    auto miss_shader = m_shader_table_->MissShader();
    dispatch_desc.MissShaderTable.StartAddress = miss_shader->GetGPUVirtualAddress();
    dispatch_desc.MissShaderTable.SizeInBytes = miss_shader->GetDesc().Width;
    dispatch_desc.MissShaderTable.StrideInBytes = dispatch_desc.MissShaderTable.SizeInBytes;

    auto hit_group_shader = m_shader_table_->HitGroupShader();
    dispatch_desc.HitGroupTable.StartAddress = hit_group_shader->GetGPUVirtualAddress();
    dispatch_desc.HitGroupTable.SizeInBytes = hit_group_shader->GetDesc().Width;
    dispatch_desc.HitGroupTable.StrideInBytes = dispatch_desc.HitGroupTable.SizeInBytes;

    dispatch_desc.Width = 1920;
    dispatch_desc.Height = 1080;
    dispatch_desc.Depth = 1;

    dxr_command_list->DispatchRays(&dispatch_desc);

    m_uav_textures_[0]->EndRender();

    m_instance_infos_.clear();
    m_vertex_buffer_handle_.clear();
    m_index_buffer_handle_.clear();
}

void engine::RaytracingManager::RegisterMeshRenderer(std::shared_ptr<MeshRenderer> mesh_renderer)
{
    const auto mesh = mesh_renderer->GetSharedMesh().CastedLock();
    if (mesh == nullptr)
    {
        m_no_mesh_mesh_renderers_.emplace_back(mesh_renderer);
        return;
    }

    Submit(mesh_renderer);
    m_mesh_renderers_.emplace_back(mesh_renderer);
}

void engine::RaytracingManager::UnRegisterMeshRenderer(const std::shared_ptr<MeshRenderer> &mesh_renderer)
{
    for (int i = 0; i < m_mesh_renderers_.size(); ++i)
    {
        if (m_mesh_renderers_[i] != mesh_renderer)
            continue;

        m_mesh_renderers_.erase(m_mesh_renderers_.begin() + i);
        m_tlas_instances_.erase(m_tlas_instances_.begin() + i);
        m_generic_material_datas_.erase(m_generic_material_datas_.begin() + i);
        m_vertex_address_buffers_.erase(m_vertex_address_buffers_.begin() + i);
        m_index_address_buffers_.erase(m_index_address_buffers_.begin() + i);

        return;
    }

    erase_if(m_mesh_renderers_, [mesh_renderer](auto &a) {
        return a == mesh_renderer;
    });
}