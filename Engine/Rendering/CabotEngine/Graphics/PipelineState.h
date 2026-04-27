#pragma once
#include "Rendering/material.h"

struct PipelineStateSettings
{
    D3D12_INPUT_LAYOUT_DESC layout_desc;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type;
    D3D12_RASTERIZER_DESC rasterizer_desc;
    D3D12_DEPTH_STENCIL_DESC depth_stencil_desc;
    D3D12_BLEND_DESC blend_desc;
    UINT num_rendertarget;
    D3D12_SHADER_BYTECODE vs_code;
    D3D12_SHADER_BYTECODE ps_code;
    D3D12_SHADER_BYTECODE gs_code = {};
    DXGI_FORMAT rtv_format[8] = {DXGI_FORMAT_UNKNOWN};
    DXGI_FORMAT dsv_format;
    UINT sample_mask;
    UINT sample_count;

    void SetShader(const engine::Shader *shader);
};

class PipelineState
{
    bool m_is_valid_ = false;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc_ = {};
    ComPtr<ID3D12PipelineState> m_p_pipeline_state_ = nullptr;
    CD3DX12_SHADER_BYTECODE m_byte_code_ = {};
    ComPtr<ID3DBlob> m_p_vs_blob_ = nullptr;
    ComPtr<ID3DBlob> m_p_ps_blob_ = nullptr;
    ComPtr<ID3DBlob> m_p_gs_blob_ = nullptr;
    
public:
    PipelineState();
    [[nodiscard]] bool IsValid() const;

    void Create(const PipelineStateSettings &setting);


    ID3D12PipelineState *Get() const;
};