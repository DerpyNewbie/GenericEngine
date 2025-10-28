#pragma once
#include "Rendering/material.h"

class PipelineState
{
public:
    PipelineState();
    bool IsValid() const;

    void SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout);
    void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_type);
    void SetRasterizerState(const D3D12_RASTERIZER_DESC &rasterizer_desc);
    void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc);
    void SetNumRenderTarget(UINT num_render_target);
    void SetVS(const std::wstring &file_path);
    void SetPS(const std::wstring &file_path);
    void SetGS(const std::wstring &file_path);
    void SetShader(const std::shared_ptr<engine::Shader> &shader); //これ呼び出したらVS,PSのセットはしなくていい
    void Create();

    ID3D12PipelineState *Get();

private:
    bool m_is_valid_ = false;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc_ = {};
    ComPtr<ID3D12PipelineState> m_p_pipeline_state_ = nullptr;
    CD3DX12_SHADER_BYTECODE m_byte_code_ = {};
    ComPtr<ID3DBlob> m_p_vs_blob_ = nullptr;
    ComPtr<ID3DBlob> m_p_ps_blob_ = nullptr;
    ComPtr<ID3DBlob> m_p_gs_blob_ = nullptr;
};