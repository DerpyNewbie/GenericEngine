#include "pch.h"
#include "ComputePipelineState.h"

#include "RenderEngine.h"
#include "RootSignature.h"

namespace engine
{
ComputePipelineState::ComputePipelineState(const ComputeShader *shader)
{
    m_desc_.pRootSignature = RootSignature::Get();

    auto byte_code = shader->GetByteCode();
    m_desc_.CS.pShaderBytecode = byte_code.pShaderBytecode;
    m_desc_.CS.BytecodeLength = byte_code.BytecodeLength;

    m_desc_.NodeMask = 0;
    m_desc_.CachedPSO.pCachedBlob = nullptr;
    m_desc_.CachedPSO.CachedBlobSizeInBytes = 0;
    m_desc_.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    auto hr = RenderEngine::Device()->CreateComputePipelineState(&m_desc_, IID_PPV_ARGS(&m_pipeline_state_));

    if (FAILED(hr))
    {
        m_is_valid_ = false;
        Logger::Error<ComputePipelineState>("Failed to create ComputePipelineState");
        return;
    }

    m_is_valid_ = true;
    return;
}

bool ComputePipelineState::IsValid() const
{
    return m_is_valid_;
}

ID3D12PipelineState *ComputePipelineState::Get() const
{
    return m_pipeline_state_.Get();
}
}