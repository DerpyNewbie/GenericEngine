#pragma once
#include "raytracing_shader.h"

class RaytracingPipelineState
{
    std::vector<ComPtr<ID3D12StateObject>> m_dxr_state_objects_;
    CD3DX12_STATE_OBJECT_DESC m_dxr_pipeline_{D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE};

public:
    static RaytracingPipelineState *Instance();
    static ComPtr<ID3D12StateObject> Get();
    void CreateDxrPipelineState(RaytracingShader raytracing_shader);
};