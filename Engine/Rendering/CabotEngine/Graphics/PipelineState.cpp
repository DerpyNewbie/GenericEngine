#include "pch.h"
#include "PipelineState.h"
#include "RenderEngine.h"
#include "RootSignature.h"

#pragma comment(lib, "d3dcompiler.lib")

PipelineState::PipelineState()
{
    // パイプラインステートの設定
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // ラスタライザーはデフォルト
    desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングはなし
    desc.RasterizerState.FrontCounterClockwise = TRUE;
    desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // ブレンドステートもデフォルト
    desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // 深度ステンシルはデフォルトを使う
    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 三角形を描画
    desc.NumRenderTargets = 1; // 描画対象は1
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.pRootSignature = engine::RootSignature::Get();
}

bool PipelineState::IsValid() const
{
    return m_IsValid;
}

void PipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
    desc.InputLayout = layout;
}

void PipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_type)
{
    desc.PrimitiveTopologyType = primitive_type;
}

void PipelineState::SetVS(std::wstring filePath)
{
    auto hr = D3DReadFileToBlob(filePath.c_str(), m_pVsBlob.GetAddressOf());
    if (FAILED(hr))
    {
        engine::Logger::Error<PipelineState>("Failed to load vertex shader");
        return;
    }

    desc.VS = CD3DX12_SHADER_BYTECODE(m_pVsBlob.Get());
}

void PipelineState::SetPS(std::wstring filePath)
{
    auto hr = D3DReadFileToBlob(filePath.c_str(), m_pPsBlob.GetAddressOf());
    if (FAILED(hr))
    {
        engine::Logger::Error<PipelineState>("Failed to load pixel shader");
        return;
    }

    desc.PS = CD3DX12_SHADER_BYTECODE(m_pPsBlob.Get());
}

void PipelineState::SetGS(std::wstring filePath)
{
    auto hr = D3DReadFileToBlob(filePath.c_str(), m_pGsBlob.GetAddressOf());
    if (FAILED(hr))
    {
        engine::Logger::Error<PipelineState>("Failed to load geometry shader");
        return;
    }

    desc.GS = CD3DX12_SHADER_BYTECODE(m_pGsBlob.Get());
}

void PipelineState::SetShader(std::shared_ptr<engine::Shader> shader)
{
    desc.VS = shader->GetByteCode(engine::Shader::ShaderType::Vertex);
    desc.PS = shader->GetByteCode(engine::Shader::ShaderType::Pixel);
}

void PipelineState::Create()
{
    auto hr = g_RenderEngine->Device()->CreateGraphicsPipelineState(
        &desc, IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        engine::Logger::Error<PipelineState>("Failed to create PipelineState");
        return;
    }

    m_IsValid = true;
}

ID3D12PipelineState *PipelineState::Get()
{
    return m_pPipelineState.Get();
}