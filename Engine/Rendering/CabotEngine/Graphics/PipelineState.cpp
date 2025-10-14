#include "pch.h"
#include "PipelineState.h"
#include "RenderEngine.h"
#include "RootSignature.h"

#pragma comment(lib, "d3dcompiler.lib")

PipelineState::PipelineState()
{
    // パイプラインステートの設定
    m_desc_.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // ラスタライザーはデフォルト
    m_desc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングはなし
    m_desc_.RasterizerState.FrontCounterClockwise = TRUE;
    m_desc_.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // ブレンドステートもデフォルト
    m_desc_.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // 深度ステンシルはデフォルトを使う
    m_desc_.SampleMask = UINT_MAX;
    m_desc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 三角形を描画
    m_desc_.NumRenderTargets = 1;
    for (auto &RTVFormat : m_desc_.RTVFormats)
        RTVFormat = DXGI_FORMAT_UNKNOWN;
    m_desc_.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    m_desc_.SampleDesc.Count = 1;
    m_desc_.SampleDesc.Quality = 0;
    m_desc_.pRootSignature = engine::RootSignature::Get();
}

bool PipelineState::IsValid() const
{
    return m_is_valid_;
}

void PipelineState::SetInputLayout(const D3D12_INPUT_LAYOUT_DESC layout)
{
    m_desc_.InputLayout = layout;
}

void PipelineState::SetPrimitiveTopologyType(const D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_type)
{
    m_desc_.PrimitiveTopologyType = primitive_type;
}

void PipelineState::SetRasterizerState(const D3D12_RASTERIZER_DESC &rasterizer_desc)
{
    m_desc_.RasterizerState = rasterizer_desc;
}

void PipelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc)
{
    m_desc_.DepthStencilState = depth_stencil_desc;
}

void PipelineState::SetNumRenderTarget(const UINT num_render_target)
{
    //RTVの数の上限を超えてたら上限数に抑える
    if (num_render_target > 8)
    {
        m_desc_.NumRenderTargets = 8;
        engine::Logger::Warn<PipelineState>("NumRenderTarget exceeds 8");
    }
    m_desc_.NumRenderTargets = num_render_target;
    for (UINT i = 0; i < m_desc_.NumRenderTargets; ++i)
    {
        m_desc_.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
    }
}

void PipelineState::SetVS(const std::wstring &file_path)
{
    auto hr = D3DReadFileToBlob(file_path.c_str(), m_p_vs_blob_.GetAddressOf());
    if (FAILED(hr))
    {
        engine::Logger::Error<PipelineState>("Failed to load vertex shader");
        return;
    }

    m_desc_.VS = CD3DX12_SHADER_BYTECODE(m_p_vs_blob_.Get());
}

void PipelineState::SetPS(const std::wstring &file_path)
{
    auto hr = D3DReadFileToBlob(file_path.c_str(), m_p_ps_blob_.GetAddressOf());
    if (FAILED(hr))
    {
        engine::Logger::Error<PipelineState>("Failed to load pixel shader");
        return;
    }

    m_desc_.PS = CD3DX12_SHADER_BYTECODE(m_p_ps_blob_.Get());
}

void PipelineState::SetGS(const std::wstring &file_path)
{
    auto hr = D3DReadFileToBlob(file_path.c_str(), m_p_gs_blob_.GetAddressOf());
    if (FAILED(hr))
    {
        engine::Logger::Error<PipelineState>("Failed to load geometry shader");
        return;
    }

    m_desc_.GS = CD3DX12_SHADER_BYTECODE(m_p_gs_blob_.Get());
}

void PipelineState::SetShader(const std::shared_ptr<engine::Shader> &shader)
{
    m_desc_.VS = shader->GetByteCode(engine::kShaderType::kShaderType_Vertex);
    m_desc_.PS = shader->GetByteCode(engine::kShaderType::kShaderType_Pixel);
}

void PipelineState::Create()
{
    auto hr = RenderEngine::Device()->CreateGraphicsPipelineState(
        &m_desc_, IID_PPV_ARGS(m_p_pipeline_state_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        engine::Logger::Error<PipelineState>("Failed to create PipelineState");
        return;
    }

    m_is_valid_ = true;
}

ID3D12PipelineState *PipelineState::Get()
{
    return m_p_pipeline_state_.Get();
}