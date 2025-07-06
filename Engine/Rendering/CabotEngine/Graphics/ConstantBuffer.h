#pragma once
#include "Rendering/buffers.h"

class ConstantBuffer : public IBuffer
{
public:
    ConstantBuffer(size_t size);
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const; // バッファのGPU上のアドレスを返す
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc(); // 定数バッファビューを返す

    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;

    bool CanUpdate() override
    {
        return true;
    }

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
    UINT64 m_SizeAligned;
    ComPtr<ID3D12Resource> m_pBuffer; // 定数バッファ
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc; // 定数バッファビューの設定
    void *m_pMappedPtr = nullptr;

    ConstantBuffer(const ConstantBuffer &) = delete;
    void operator =(const ConstantBuffer &) = delete;
};