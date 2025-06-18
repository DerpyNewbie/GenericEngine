#pragma once
#include <directx/d3dx12.h>
#include "ComPtr.h"

class ConstantBuffer
{
public:
    ConstantBuffer(size_t size); // コンストラクタで定数バッファを生成
    bool IsValid(); // バッファ生成に成功したかを返す
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const; // バッファのGPU上のアドレスを返す
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc(); // 定数バッファビューを返す

    void *GetPtr() const
    {
        return m_pMappedPtr;
    } // 定数バッファにマッピングされたポインタを返す

    template <typename T>
    T *GetPtr()
    {
        return static_cast<T *>(GetPtr());
    }

private:
    bool m_IsValid = false; // 定数バッファ生成に成功したか
    ComPtr<ID3D12Resource> m_pBuffer; // 定数バッファ
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc; // 定数バッファビューの設定
    void *m_pMappedPtr = nullptr;

    ConstantBuffer(const ConstantBuffer &) = delete;
    void operator =(const ConstantBuffer &) = delete;
};