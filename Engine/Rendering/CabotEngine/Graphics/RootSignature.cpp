#include "pch.h"

#include "RootSignature.h"
#include "RenderEngine.h"

namespace engine
{

std::shared_ptr<RootSignature> RootSignature::Instance()
{
    static auto instance = std::make_shared<RootSignature>();
    return instance;
}

RootSignature::RootSignature()
{
    auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

    std::vector<CD3DX12_ROOT_PARAMETER> rootParam(kRootParameterIndexCount);

    rootParam[kWorldCBV].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
    rootParam[kViewProjCBV].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
    CD3DX12_DESCRIPTOR_RANGE tableRangeBone = {};
    tableRangeBone.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    rootParam[kBoneSRV].InitAsDescriptorTable(1, &tableRangeBone, D3D12_SHADER_VISIBILITY_ALL);
    
    CD3DX12_DESCRIPTOR_RANGE tableRangeLightVP = {};
    tableRangeLightVP.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    rootParam[kLightViewProj].InitAsDescriptorTable(1, &tableRangeLightVP, D3D12_SHADER_VISIBILITY_ALL);
    
    CD3DX12_DESCRIPTOR_RANGE tableRangeShadowMap = {};
    tableRangeShadowMap.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
    rootParam[kShadowMapSRV].InitAsDescriptorTable(1, &tableRangeShadowMap, D3D12_SHADER_VISIBILITY_ALL);

    CD3DX12_DESCRIPTOR_RANGE tableRangeVSCBV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangeVSSRV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangeVSUAV = {};

    tableRangeVSCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 20, 2);
    tableRangeVSSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 3);
    tableRangeVSUAV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 6, 1);

    rootParam[kMaterialCBV].InitAsDescriptorTable(1, &tableRangeVSCBV, D3D12_SHADER_VISIBILITY_ALL);
    rootParam[kMaterialSRV].InitAsDescriptorTable(1, &tableRangeVSSRV, D3D12_SHADER_VISIBILITY_ALL);
    rootParam[kMaterialUAV].InitAsDescriptorTable(1, &tableRangeVSUAV, D3D12_SHADER_VISIBILITY_ALL);
    
    D3D12_STATIC_SAMPLER_DESC sampler[2];
    sampler[0] = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

    sampler[1] = CD3DX12_STATIC_SAMPLER_DESC(
        1,
        D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
    0.0f,
    16,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
    D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
    );

    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = static_cast<UINT>(std::size(rootParam));
    desc.NumStaticSamplers = 2;
    desc.pParameters = rootParam.data();
    desc.pStaticSamplers = sampler;
    desc.Flags = flag;

    ComPtr<ID3DBlob> pBlob;
    ComPtr<ID3DBlob> pErrorBlob;

    auto hr = D3D12SerializeRootSignature(
    &desc,
    D3D_ROOT_SIGNATURE_VERSION_1_0,
    pBlob.GetAddressOf(),
    pErrorBlob.GetAddressOf());
    if (FAILED(hr))
    {
        Logger::Error("Failed to serialize RootSignature");
        return;
    }

    hr = RenderEngine::Device()->CreateRootSignature(
    0,
    pBlob->GetBufferPointer(),
    pBlob->GetBufferSize(),
    IID_PPV_ARGS(m_root_signature_.GetAddressOf()));
    if (FAILED(hr))
    {
        Logger::Error("Failed to Create RootSignature");
        return;
    }

    m_is_valid_ = true;
}

bool RootSignature::IsValid()
{
    return Instance()->m_root_signature_ != nullptr;
}

ID3D12RootSignature *RootSignature::Get()
{
    return Instance()->m_root_signature_.Get();
}
}