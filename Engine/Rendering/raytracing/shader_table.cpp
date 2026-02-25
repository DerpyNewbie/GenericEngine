#include "pch.h"
#include "shader_table.h"

#include "engine_util.h"
#include "raytracing_pipeline_state.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
ShaderTable::ShaderTable()
{
    RaytracingPipelineState::Get().As(&m_state_object_prop_);

    void *ray_gen_id = m_state_object_prop_->GetShaderIdentifier(L"MyRayGen");
    void *miss_id = m_state_object_prop_->GetShaderIdentifier(L"MyMiss");
    void *hit_id = m_state_object_prop_->GetShaderIdentifier(L"MyHitGroup");

    UINT shader_id_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
    UINT local_root_arguments_size = 0;
    UINT record_size = shader_id_size + local_root_arguments_size;

    record_size = EngineUtil::Align(record_size, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);

    auto upload_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(record_size);

    RenderEngine::Device()->CreateCommittedResource(
        &upload_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_ray_gen_shader_table_)
    );

    uint8_t *mapped_data = nullptr;
    m_ray_gen_shader_table_->Map(0, nullptr, reinterpret_cast<void **>(&mapped_data));

    memcpy(mapped_data, ray_gen_id, shader_id_size);

    m_ray_gen_shader_table_->Unmap(0, nullptr);

    RenderEngine::Device()->CreateCommittedResource(
        &upload_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_miss_shader_table_)
    );

    mapped_data = nullptr;
    m_miss_shader_table_->Map(0, nullptr, reinterpret_cast<void **>(&mapped_data));

    memcpy(mapped_data, miss_id, shader_id_size);

    m_miss_shader_table_->Unmap(0, nullptr);

    RenderEngine::Device()->CreateCommittedResource(
        &upload_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_hit_group_shader_table_)
    );

    mapped_data = nullptr;
    m_hit_group_shader_table_->Map(0, nullptr, reinterpret_cast<void **>(&mapped_data));

    memcpy(mapped_data, hit_id, shader_id_size);

    m_hit_group_shader_table_->Unmap(0, nullptr);
}

ComPtr<ID3D12Resource> ShaderTable::RayGenShader()
{
    return m_ray_gen_shader_table_;
}

ComPtr<ID3D12Resource> ShaderTable::MissShader()
{
    return m_miss_shader_table_;
}

ComPtr<ID3D12Resource> ShaderTable::HitGroupShader()
{
    return m_hit_group_shader_table_;
}
}