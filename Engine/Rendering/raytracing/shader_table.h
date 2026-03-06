#pragma once
#include "raytracing_shader.h"

namespace engine
{
class ShaderTable
{
    ComPtr<ID3D12StateObjectProperties> m_state_object_prop_;
    ComPtr<ID3D12Resource> m_ray_gen_shader_table_;
    ComPtr<ID3D12Resource> m_miss_shader_table_;
    ComPtr<ID3D12Resource> m_hit_group_shader_table_;

public:
    ShaderTable(const std::shared_ptr<RaytracingShader> &raytracing_shader);

    ComPtr<ID3D12Resource> RayGenShader();
    ComPtr<ID3D12Resource> MissShader();
    ComPtr<ID3D12Resource> HitGroupShader();

};
}