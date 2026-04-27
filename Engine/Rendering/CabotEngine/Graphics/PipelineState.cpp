#include "pch.h"
#include "PipelineState.h"
#include "RenderEngine.h"
#include "RootSignature.h"

#pragma comment(lib, "d3dcompiler.lib")

void PipelineStateSettings::SetShader(const engine::Shader *shader)
{
    vs_code = shader->GetByteCode(engine::kShaderType::kShaderType_Vertex);
    ps_code = shader->GetByteCode(engine::kShaderType::kShaderType_Pixel);
    gs_code = shader->GetByteCode(engine::kShaderType::kShaderType_Geometry);

    auto shader_setting = shader->ShaderSettings();
    rasterizer_desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    rasterizer_desc.CullMode = DX_Cull[shader_setting.cull];

    depth_stencil_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    depth_stencil_desc.DepthFunc = DX_ZTest[shader_setting.z_test];
    depth_stencil_desc.DepthWriteMask = DX_ZWrite[shader_setting.z_write];

    blend_desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    auto &rt = blend_desc.RenderTarget[0];
    rt.BlendEnable = shader_setting.use_blend;
    rt.SrcBlend = DX_BlendFactor[shader_setting.blend_src];
    rt.DestBlend = DX_BlendFactor[shader_setting.blend_dst];
    rt.BlendOp = DX_BlendOp[shader_setting.blend_op];
    rt.RenderTargetWriteMask = DX_ColorMask[shader_setting.color_mask];

    blend_desc.AlphaToCoverageEnable = shader_setting.alpha_to_mask;

    num_rendertarget = 1;
    dsv_format = DXGI_FORMAT_D32_FLOAT;

    sample_mask = UINT_MAX;
    sample_count = 1;
    primitive_topology_type = DX_PrimitiveTopologyType[shader_setting.primitive_topology_type];
}

PipelineState::PipelineState()
{
    m_desc_.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    m_desc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    m_desc_.RasterizerState.FrontCounterClockwise = TRUE;
    m_desc_.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    m_desc_.SampleMask = UINT_MAX;
    m_desc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
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

void PipelineState::Create(const PipelineStateSettings &setting)
{
    m_desc_.InputLayout = setting.layout_desc;
    m_desc_.PrimitiveTopologyType = setting.primitive_topology_type;
    m_desc_.RasterizerState = setting.rasterizer_desc;
    m_desc_.DepthStencilState = setting.depth_stencil_desc;
    m_desc_.BlendState = setting.blend_desc;

    m_desc_.NumRenderTargets = setting.num_rendertarget;
    if (m_desc_.NumRenderTargets > D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT)
    {
        m_desc_.NumRenderTargets = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;
        engine::Logger::Warn<PipelineState>("NumRenderTarget exceeds 8");
    }

    for (UINT i = 0; i < setting.num_rendertarget; ++i)
    {
        m_desc_.RTVFormats[i] = setting.rtv_format[i];
    }

    m_desc_.VS = setting.vs_code;
    m_desc_.PS = setting.ps_code;
    m_desc_.GS = setting.gs_code;

    m_desc_.DSVFormat = setting.dsv_format;
    m_desc_.SampleMask = setting.sample_mask;
    m_desc_.SampleDesc.Count = setting.sample_count;

    auto hr = RenderEngine::Device()->CreateGraphicsPipelineState(
        &m_desc_, IID_PPV_ARGS(m_p_pipeline_state_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        engine::Logger::Error<PipelineState>("Failed to create PipelineState");
        return;
    }

    m_is_valid_ = true;
}

ID3D12PipelineState *PipelineState::Get() const
{
    return m_p_pipeline_state_.Get();
}