#include "pch.h"

#include <comdef.h>
#include "raytracing_pipeline_state.h"
#include "raytracing_global_root_signature.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
RaytracingPipelineState *RaytracingPipelineState::Instance()
{
    static auto instance = new RaytracingPipelineState;
    return instance;
}

ComPtr<ID3D12StateObject> RaytracingPipelineState::Get(const std::shared_ptr<RaytracingShader> &raytracing_shader)
{
    return Instance()->m_dxr_state_objects_[raytracing_shader];
}

void RaytracingPipelineState::CreateDxrPipelineState(const std::shared_ptr<RaytracingShader> &raytracing_shader)
{
    CD3DX12_STATE_OBJECT_DESC dxr_pipeline(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);
    const auto lib = dxr_pipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

    const D3D12_SHADER_BYTECODE shader_byte_code = raytracing_shader->GetByteCode();
    lib->SetDXILLibrary(&shader_byte_code);

    lib->DefineExport(L"MyRayGen");
    lib->DefineExport(L"MyMiss");
    lib->DefineExport(L"MyClosestHit");

    const auto hit_group = dxr_pipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    hit_group->SetClosestHitShaderImport(L"MyClosestHit");
    hit_group->SetHitGroupExport(L"MyHitGroup");
    hit_group->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    const auto shader_config = dxr_pipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    shader_config->Config(sizeof(float) * 4, sizeof(float) * 2);

    const auto pipeline_config = dxr_pipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    pipeline_config->Config(1);

    const auto global_root_sig = dxr_pipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    global_root_sig->SetRootSignature(RaytracingGlobalRootSignature::Instance()->Get());

    ComPtr<ID3D12StateObject> dxr_state_object;
    auto hr = RenderEngine::DxrDevice()->CreateStateObject(dxr_pipeline, IID_PPV_ARGS(&dxr_state_object));

    if (FAILED(hr))
    {
        _com_error err(hr);
        OutputDebugString(err.ErrorMessage());
    }

    m_dxr_state_objects_[raytracing_shader] = dxr_state_object;
}
}