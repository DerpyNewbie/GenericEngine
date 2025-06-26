#include "pch.h"

#include "PipelineState.h"
#include "RenderEngine.h"
#include <directx/d3dx12.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

PipelineState::PipelineState()
{
    // パイプラインステートの設定
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // ラスタライザーはデフォルト
    desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングはなし
    desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // ブレンドステートもデフォルト
    desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // 深度ステンシルはデフォルトを使う
    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 三角形を描画
    desc.NumRenderTargets = 1; // 描画対象は1
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
}

bool PipelineState::IsValid()
{
    return m_IsValid;
}

void PipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
    desc.InputLayout = layout;
}

void PipelineState::SetRootSignature(ID3D12RootSignature *rootSignature)
{
    desc.pRootSignature = rootSignature;
}

void PipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_type)
{
    desc.PrimitiveTopologyType = primitive_type;
}

void PipelineState::SetVS(std::wstring filePath)
{
    // 頂点シェーダー読み込み
    auto hr = D3DReadFileToBlob(filePath.c_str(), m_pVsBlob.GetAddressOf());
    if (FAILED(hr))
    {
        printf("頂点シェーダーの読み込みに失敗");
        return;
    }

    desc.VS = CD3DX12_SHADER_BYTECODE(m_pVsBlob.Get());
}

void PipelineState::SetPS(std::wstring filePath)
{
    // ピクセルシェーダー読み込み
    auto hr = D3DReadFileToBlob(filePath.c_str(), m_pPsBlob.GetAddressOf());
    if (FAILED(hr))
    {
        printf("ピクセルシェーダーの読み込みに失敗");
        return;
    }

    desc.PS = CD3DX12_SHADER_BYTECODE(m_pPsBlob.Get());
}

void PipelineState::SetGS(std::wstring filePath)
{
    auto hr = D3DReadFileToBlob(filePath.c_str(), m_pGsBlob.GetAddressOf());
    if (FAILED(hr))
    {
        printf("ピクセルシェーダーの読み込みに失敗");
        return;
    }

    desc.GS = CD3DX12_SHADER_BYTECODE(m_pGSBlob.Get());
}

void PipelineState::SetMaterial(std::shared_ptr<engine::Material> material)
{
    desc.VS = material->vertex_shader->GetByteCode();
    desc.PS = material->pixel_shader->GetByteCode();
}

void PipelineState::Create()
{
    // パイプラインステートを生成
    auto hr = g_RenderEngine->Device()->CreateGraphicsPipelineState(
        &desc, IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        printf("パイプラインステートの生成に失敗");
        return;
    }

    m_IsValid = true;
}

ID3D12PipelineState *PipelineState::Get()
{
    return m_pPipelineState.Get();
}