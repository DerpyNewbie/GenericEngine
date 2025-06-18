#pragma once
#include "ComPtr.h"

struct ID3D12RootSignature;

class RootSignature
{
    static constexpr int ConstantBufferMaxSize = 6;
    static constexpr int DescriptorHeapMaxSize = 6;
public:
    RootSignature(); // コンストラクタでルートシグネチャを生成
    bool IsValid(); // ルートシグネチャの生成に成功したかどうかを返す
    ID3D12RootSignature* Get(); // ルートシグネチャを返す

private:
    bool m_IsValid = false; // ルートシグネチャの生成に成功したかどうか
    ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr; // ルートシグネチャ
};
