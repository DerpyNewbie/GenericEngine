#pragma once
#include "raytracing_shader.h"

namespace engine
{
class RaytracingPipelineState
{
    std::unordered_map<std::shared_ptr<RaytracingShader>, ComPtr<ID3D12StateObject>> m_dxr_state_objects_;

public:
    static RaytracingPipelineState *Instance();
    static ComPtr<ID3D12StateObject> Get(const std::shared_ptr<RaytracingShader> &raytracing_shader);
    void CreateDxrPipelineState(const std::shared_ptr<RaytracingShader> &raytracing_shader);
};
}