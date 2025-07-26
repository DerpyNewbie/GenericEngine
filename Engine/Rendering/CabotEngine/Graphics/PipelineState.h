#pragma once
#include "Rendering/material.h"

class PipelineState
{
public:
    PipelineState(); // コンストラクタである程度の設定をする
    bool IsValid(); // 生成に成功したかどうかを返す

    void SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout);
    void SetRootSignature(ID3D12RootSignature *rootSignature);
    void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_type);
    void SetRasterizerState(const D3D12_RASTERIZER_DESC &rasterizer_desc);
    void SetDepthStencilState(D3D12_DEPTH_STENCIL_DESC depth_stencil_desc);

    void SetVS(std::wstring filePath);
    void SetPS(std::wstring filePath);
    void SetGS(std::wstring filePath);
    void SetShader(std::shared_ptr<engine::Shader> shader); //これ呼び出したらVS,PSのセットはしなくていい
    void Create();

    ID3D12PipelineState *Get();

private:
    bool m_IsValid = false; // 生成に成功したかどうか
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {}; // パイプラインステートの設定
    ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr; // パイプラインステート
    CD3DX12_SHADER_BYTECODE m_ByteCode = {};
    ComPtr<ID3DBlob> m_pVsBlob = nullptr;
    ComPtr<ID3DBlob> m_pPsBlob = nullptr;
    ComPtr<ID3DBlob> m_pGsBlob = nullptr;
};