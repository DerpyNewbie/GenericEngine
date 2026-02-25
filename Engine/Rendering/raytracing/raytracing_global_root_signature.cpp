#include "pch.h"
#include "raytracing_global_root_signature.h"

#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

RaytracingGlobalRootSignature *RaytracingGlobalRootSignature::Instance()
{
    static auto instance = new RaytracingGlobalRootSignature();
    return instance;
}
ID3D12RootSignature *RaytracingGlobalRootSignature::Get()
{
    return Instance()->m_root_signature_.Get();
}

RaytracingGlobalRootSignature::RaytracingGlobalRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE range;
    range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

    CD3DX12_ROOT_PARAMETER root_params[2];
    root_params[0].InitAsDescriptorTable(1, &range);

    root_params[1].InitAsShaderResourceView(0);

    CD3DX12_ROOT_SIGNATURE_DESC global_root_sig_desc(2, root_params, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);
    
    ComPtr<ID3DBlob> error;
    auto hr = D3D12SerializeRootSignature(&global_root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, &m_signature_, &error);

    if (FAILED(hr))
    {
        if (error != nullptr)
        {
            OutputDebugStringA(static_cast<char *>(error->GetBufferPointer()));
        }
        return;
    }
    
    RenderEngine::Device()->CreateRootSignature(0, m_signature_->GetBufferPointer(), m_signature_->GetBufferSize(), IID_PPV_ARGS(&m_root_signature_));
}