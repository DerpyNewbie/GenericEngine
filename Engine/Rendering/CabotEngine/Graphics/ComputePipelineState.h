#pragma once
#include "Rendering/compute_shader.h"
#include "Rendering/material.h"

namespace engine
{
class ComputePipelineState
{
    bool m_is_valid_ = false;
    D3D12_COMPUTE_PIPELINE_STATE_DESC m_desc_ = {};
    ComPtr<ID3D12PipelineState> m_pipeline_state_ = nullptr;
    CD3DX12_SHADER_BYTECODE m_byte_code_ = {};
    ComPtr<ID3DBlob> m_blob_ = nullptr;

public:
    ComputePipelineState(const ComputeShader *shader);
    [[nodiscard]] bool IsValid() const;

    ID3D12PipelineState *Get() const;
};
}