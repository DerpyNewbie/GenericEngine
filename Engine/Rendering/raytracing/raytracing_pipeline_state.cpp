#include "pch.h"
#include "raytracing_pipeline_state.h"

#include "raytracing_global_root_signature.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

void RaytracingPipelineState::CreateDxrPipelineState(RaytracingShader raytracing_shader)
{
    auto lib = m_dxr_pipeline_.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

    D3D12_SHADER_BYTECODE shader_byte_code = raytracing_shader.GetByteCode();
    lib->SetDXILLibrary(&shader_byte_code);

    lib->DefineExport(L"MyRayGen");
    lib->DefineExport(L"MyMiss");
    lib->DefineExport(L"MyClosestHit");

    auto hit_group = m_dxr_pipeline_.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    hit_group->SetClosestHitShaderImport(L"MyClosestHit");
    hit_group->SetHitGroupExport(L"MyHitGroup");
    hit_group->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    auto shader_config = m_dxr_pipeline_.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    shader_config->Config(sizeof(float) * 4, sizeof(float) * 2);

    auto pipeline_config = m_dxr_pipeline_.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    pipeline_config->Config(1);

    auto global_root_sig = m_dxr_pipeline_.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    global_root_sig->SetRootSignature(RaytracingGlobalRootSignature::Instance()->Get());

    ComPtr<ID3D12StateObject> dxr_state_object;
    RenderEngine::DxrDevice()->CreateStateObject(m_dxr_pipeline_, IID_PPV_ARGS(&dxr_state_object));

    m_dxr_state_objects_.emplace_back(dxr_state_object);
}