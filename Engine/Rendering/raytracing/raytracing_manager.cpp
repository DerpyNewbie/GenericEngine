#include "pch.h"
#include "raytracing_manager.h"

#include "raytracing_global_root_signature.h"
#include "raytracing_pipeline_state.h"
#include "Components/camera_component.h"
#include "Rendering/render_pipeline.h"

void engine::RaytracingManager::ExecuteRequest(const RaytracingRequest &raytracing_request)
{
    const auto current_back_buffer_index = RenderEngine::CurrentBackBufferIndex();

    const auto uav_texture = raytracing_request.uav_texture;
    const auto uav_handle = raytracing_request.uav_handle;
    const auto raytracing_shader = raytracing_request.raytracing_shader;
    const auto shader_table = raytracing_request.shader_table;
    const auto target_camera = raytracing_request.target_camera;
    const auto view_proj_buffer = *m_view_proj_matrix_buffers_[current_back_buffer_index].Get();

    uav_texture->BeginRender();

    const auto dxr_command_list = RenderEngine::DxrCommandList();

    const auto view = target_camera->ViewMatrix().Invert();
    const auto proj = target_camera->property.ProjectionMatrix().Invert();

    ViewProjection view_projection;
    view_projection.matrices[0] = view;
    view_projection.matrices[1] = proj;
    view_proj_buffer->UpdateBuffer(&view_projection);

    dxr_command_list->SetPipelineState1(RaytracingPipelineState::Get(raytracing_shader).Get());
    dxr_command_list->SetComputeRootSignature(RaytracingGlobalRootSignature::Get());
    dxr_command_list->SetComputeRootConstantBufferView(0, view_proj_buffer->GetAddress());
    dxr_command_list->SetComputeRootDescriptorTable(1, uav_handle.HandleGPU);
    dxr_command_list->SetComputeRootShaderResourceView(2, m_tlas_->GetGPUVirtualAddress());
    dxr_command_list->SetComputeRootShaderResourceView(3, m_material_buffer_->GetAddress());
    dxr_command_list->SetComputeRootShaderResourceView(4, m_instance_info_buffer_->GetAddress());
    dxr_command_list->SetComputeRootDescriptorTable(5, m_vertex_buffer_handle_[0].HandleGPU);

    D3D12_DISPATCH_RAYS_DESC dispatch_desc = {};

    const auto ray_gen_shader = shader_table->RayGenShader();
    dispatch_desc.RayGenerationShaderRecord.StartAddress = ray_gen_shader->GetGPUVirtualAddress();
    dispatch_desc.RayGenerationShaderRecord.SizeInBytes = ray_gen_shader->GetDesc().Width;

    const auto miss_shader = shader_table->MissShader();
    dispatch_desc.MissShaderTable.StartAddress = miss_shader->GetGPUVirtualAddress();
    dispatch_desc.MissShaderTable.SizeInBytes = miss_shader->GetDesc().Width;
    dispatch_desc.MissShaderTable.StrideInBytes = dispatch_desc.MissShaderTable.SizeInBytes;

    const auto hit_group_shader = shader_table->HitGroupShader();
    dispatch_desc.HitGroupTable.StartAddress = hit_group_shader->GetGPUVirtualAddress();
    dispatch_desc.HitGroupTable.SizeInBytes = hit_group_shader->GetDesc().Width;
    dispatch_desc.HitGroupTable.StrideInBytes = dispatch_desc.HitGroupTable.SizeInBytes;

    dispatch_desc.Width = 1920;
    dispatch_desc.Height = 1080;
    dispatch_desc.Depth = 1;

    dxr_command_list->DispatchRays(&dispatch_desc);

    uav_texture->EndRender();
}

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

    for (int i = 0; i < m_requests_.size(); ++i)
    {
        m_requests_[i].uav_handle = RenderPipeline::GetDynamicDescriptorHeap()->Allocate();
        m_requests_[i].uav_texture->UploadBuffer(m_requests_[i].uav_handle);
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
    instance->m_tlas_ = std::make_shared<TopLevelAccelerationStructure>();

    instance->m_material_buffer_ = std::make_shared<StructuredBuffer>(sizeof(GenericMaterialData), 10);
    instance->m_material_buffer_->CreateBuffer();

    instance->m_instance_info_buffer_ = std::make_shared<StructuredBuffer>(sizeof(InstanceInfo), 10);
    instance->m_instance_info_buffer_->CreateBuffer();

    for (auto &view_proj_matrix_buffers : instance->m_view_proj_matrix_buffers_)
    {
        view_proj_matrix_buffers.SetMaxSize(kStableCameraCount);
    }
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

    for (auto &request : m_requests_)
    {
        ExecuteRequest(request);
    }
    
    m_instance_infos_.clear();
    m_vertex_buffer_handle_.clear();
    m_index_buffer_handle_.clear();
    m_requests_.clear();
    for (auto &view_proj_matrix_buffers : m_view_proj_matrix_buffers_)
    {
        view_proj_matrix_buffers.ReturnAll();
    }
}

void engine::RaytracingManager::RequestRaytracing(const std::shared_ptr<CameraComponent> &target_camera, const std::shared_ptr<RaytracingShader> &raytracing_shader, const std::shared_ptr<ShaderTable> &shader_table, const std::shared_ptr<UavTexture> &uav_texture)
{
    if (raytracing_shader == nullptr || shader_table == nullptr || uav_texture == nullptr)
        return;

    RaytracingRequest raytracing_request;
    raytracing_request.raytracing_shader = raytracing_shader;
    raytracing_request.shader_table = shader_table;
    raytracing_request.uav_texture = uav_texture;
    raytracing_request.target_camera = target_camera;

    Instance()->m_requests_.emplace_back(raytracing_request);
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