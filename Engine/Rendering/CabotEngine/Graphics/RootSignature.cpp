#include "pch.h"

#include "RootSignature.h"
#include "RenderEngine.h"
#include <directx/d3dx12.h>
#include <array>

RootSignature::RootSignature()
{
    auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // アプリケーションの入力アセンブラを使用する
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS; // ドメインシェーダーのルートシグネチャへんアクセスを拒否する
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS; // ハルシェーダーのルートシグネチャへんアクセスを拒否する
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS; // ジオメトリシェーダーのルートシグネチャへんアクセスを拒否する

    std::vector<CD3DX12_ROOT_PARAMETER> rootParam(ConstantBufferMaxSize + 4);

    for (int i = 0; i < ConstantBufferMaxSize; ++i)
    {
        rootParam[i].InitAsConstantBufferView(i, 0, D3D12_SHADER_VISIBILITY_ALL);
    }

    //DescriptorHeapの作成
    CD3DX12_DESCRIPTOR_RANGE tableRangeVSSRV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangeVSCBV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangePSSRV = {};
    CD3DX12_DESCRIPTOR_RANGE tableRangePSCBV = {};
    
    tableRangeVSSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 0);
    tableRangeVSCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 20, 0);//CBVはちょっと多めに確保
    tableRangePSSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 0);
    tableRangePSCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 20, 0);
    
    rootParam[ConstantBufferMaxSize].InitAsDescriptorTable(1, &tableRangeVSSRV, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParam[ConstantBufferMaxSize + 1].InitAsDescriptorTable(1, &tableRangeVSCBV, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParam[ConstantBufferMaxSize + 2].InitAsDescriptorTable(1, &tableRangePSSRV, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParam[ConstantBufferMaxSize + 3].InitAsDescriptorTable(1, &tableRangePSCBV, D3D12_SHADER_VISIBILITY_PIXEL);

    // スタティックサンプラーの設定
    D3D12_STATIC_SAMPLER_DESC sampler[2];
    sampler[0] = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

    sampler[1] = CD3DX12_STATIC_SAMPLER_DESC(1, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
    sampler[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

    // ルートシグニチャの設定（設定したいルートパラメーターとスタティックサンプラーを入れる）
    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = static_cast<UINT>(std::size(rootParam)); // ルートパラメーターの個数をいれる
    desc.NumStaticSamplers = 2; // サンプラーの個数をいれる
    desc.pParameters = rootParam.data(); // ルートパラメーターのポインタをいれる
    desc.pStaticSamplers = sampler; // サンプラーのポインタを入れる
    desc.Flags = flag; // フラグを設定

    ComPtr<ID3DBlob> pBlob;
    ComPtr<ID3DBlob> pErrorBlob;

    // シリアライズ
    auto hr = D3D12SerializeRootSignature(
        &desc,
        D3D_ROOT_SIGNATURE_VERSION_1_0,
        pBlob.GetAddressOf(),
        pErrorBlob.GetAddressOf());
    if (FAILED(hr))
    {
        printf("ルートシグネチャシリアライズに失敗");
        return;
    }

    // ルートシグネチャ生成
    hr = g_RenderEngine->Device()->CreateRootSignature(
        0, // GPUが複数ある場合のノードマスク（今回は1個しか無い想定なので0）
        pBlob->GetBufferPointer(), // シリアライズしたデータのポインタ
        pBlob->GetBufferSize(), // シリアライズしたデータのサイズ
        IID_PPV_ARGS(m_pRootSignature.GetAddressOf())); // ルートシグニチャ格納先のポインタ
    if (FAILED(hr))
    {
        printf("ルートシグネチャの生成に失敗");
        return;
    }

    m_IsValid = true;
}

bool RootSignature::IsValid()
{
    return m_IsValid;
}

ID3D12RootSignature *RootSignature::Get()
{
    return m_pRootSignature.Get();
}