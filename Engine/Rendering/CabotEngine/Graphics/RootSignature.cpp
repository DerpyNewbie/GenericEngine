#include "pch.h"

#include "RootSignature.h"
#include "RenderEngine.h"

namespace engine
{
std::shared_ptr<RootSignature> RootSignature::m_instance_;

std::shared_ptr<RootSignature> RootSignature::Instance()
{
    if (!m_instance_)
    {
        m_instance_ = std::make_shared<RootSignature>();
    }
    return m_instance_;
}

RootSignature::RootSignature()
{
    auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    std::vector<CD3DX12_ROOT_PARAMETER> rootParam(kRootParameterIndexCount);

    rootParam[kWorldCBV].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
    rootParam[kViewProjCBV].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
    rootParam[kLightCountCBV].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
    rootParam[kBoneSRV].InitAsShaderResourceView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
    CD3DX12_DESCRIPTOR_RANGE tableRangeLight = {};
    tableRangeLight.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    rootParam[kLightSRV].InitAsDescriptorTable(1, &tableRangeLight, D3D12_SHADER_VISIBILITY_ALL);

    CD3DX12_DESCRIPTOR_RANGE tableRangeShadowMap = {};
    tableRangeShadowMap.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
    rootParam[kShadowMapSRV].InitAsDescriptorTable(1, &tableRangeShadowMap, D3D12_SHADER_VISIBILITY_ALL);

    CD3DX12_DESCRIPTOR_RANGE tableRangeVSCBV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangeVSSRV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangeVSUAV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangePSCBV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangePSSRV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangePSUAV = {};

    tableRangeVSCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 20, 3);
    tableRangeVSSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 3);
    tableRangeVSUAV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 6, 0);
    tableRangePSCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 20, 3);
    tableRangePSSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 3);
    tableRangePSUAV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 6, 0);

    rootParam[kVertexCBV].InitAsDescriptorTable(1, &tableRangeVSCBV, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParam[kVertexSRV].InitAsDescriptorTable(1, &tableRangeVSSRV, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParam[kVertexUAV].InitAsDescriptorTable(1, &tableRangeVSUAV, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParam[kPixelCBV].InitAsDescriptorTable(1, &tableRangePSCBV, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParam[kPixelSRV].InitAsDescriptorTable(1, &tableRangePSSRV, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParam[kPixelUAV].InitAsDescriptorTable(1, &tableRangePSUAV, D3D12_SHADER_VISIBILITY_PIXEL);

    // スタティックサンプラーの設定
    D3D12_STATIC_SAMPLER_DESC sampler[2];
    sampler[0] = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

    sampler[1] = CD3DX12_STATIC_SAMPLER_DESC(1, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
    sampler[1].AddressU = sampler[1].AddressV = sampler[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

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

    // ルートシグネチャ生成
    hr = RenderEngine::Device()->CreateRootSignature(
        0,
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        IID_PPV_ARGS(m_pRootSignature.GetAddressOf()));
    if (FAILED(hr))
    {
        Logger::Error("Failed to Create RootSignature");
        return;
    }

    m_IsValid = true;
}

bool RootSignature::IsValid()
{
    return Instance()->m_IsValid;
}

ID3D12RootSignature *RootSignature::Get()
{
    return Instance()->m_pRootSignature.Get();
}
}