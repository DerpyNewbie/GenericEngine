#include "pch.h"
#include "raytracing_shader.h"

CD3DX12_SHADER_BYTECODE RaytracingShader::GetByteCode() const
{
    return m_shader_blob_.Get();
}