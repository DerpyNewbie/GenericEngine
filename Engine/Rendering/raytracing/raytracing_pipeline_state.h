#pragma once
#include "raytracing_shader.h"

class RaytracingPipelineState
{
    std::vector<ComPtr<ID3D12StateObject>> m_dxr_state_objects_;

public:
    static RaytracingPipelineState *Instance();
    static ComPtr<ID3D12StateObject> Get();
    void CreateDxrPipelineState(const RaytracingShader &raytracing_shader);
};