#include "pch.h"
#include "raytracing_local_root_signature.h"

#include "Rendering/CabotEngine/Graphics/RenderEngine.h"


RaytracingLocalRootSignature *RaytracingLocalRootSignature::Instance()
{
    static auto instance = new RaytracingLocalRootSignature();
    return instance;
}

ID3D12RootSignature *RaytracingLocalRootSignature::Get()
{
    return Instance()->m_root_signature_.Get();
}

RaytracingLocalRootSignature::RaytracingLocalRootSignature()
{
    CD3DX12_ROOT_PARAMETER local_params[1];
    local_params[0].InitAsConstants(sizeof(Color) / 4, 0, 1);

    CD3DX12_ROOT_SIGNATURE_DESC local_root_sig_desc(1, local_params, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

    ComPtr<ID3DBlob> error;
    D3D12SerializeRootSignature(&local_root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, &m_signature_, &error);
    RenderEngine::Device()->CreateRootSignature(0, m_signature_->GetBufferPointer(), m_signature_->GetBufferSize(), IID_PPV_ARGS(&m_root_signature_));
}