#pragma once
#include "Rendering/ibuffer.h"

/// <summary>
/// Shaderに渡す定数Bufferです。
/// </summary>
class ConstantBuffer : public IBuffer
{
    uint64_t m_size_aligned_;
    uint64_t m_size_;

    ComPtr<ID3D12Resource> m_buffer_;
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_desc_;

    void *m_p_mapped_ptr_ = nullptr;

public:
    /// <summary>
    /// 指定されたサイズのConstantBufferを準備します。サイズは256byte単位に切り上げられます。
    /// </summary>
    /// <param name="size">データのサイズ(byte)</param>
    explicit ConstantBuffer(size_t size);

    /// <summary>
    /// Upload用のHeapにBufferを作成し、書き込めるようにMapします。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// データをBufferにコピーします。
    /// </summary>
    /// <param name="data">書き込むデータ</param>
    void UpdateBuffer(void *data) override;
    /// <summary>
    /// ConstantBufferViewをDescriptorHeapに登録します。
    /// </summary>
    /// <returns>登録されたDescriptorHandle</returns>
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    /// <summary>
    /// Bufferが作成済みであるかどうかを取得します。
    /// </summary>
    bool IsValid() override;

    /// <summary>
    /// 常に true を返します。
    /// </summary>
    bool CanUpdate() override;

    /// <summary>
    /// Bufferの書き込み先のメモリへのポインタを取得します。
    /// </summary>
    void *GetPtr() const;

    /// <summary>
    /// BufferのGPU上のアドレスを取得します。
    /// </summary>
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
    /// <summary>
    /// ConstantBufferViewの設定を取得します。
    /// </summary>
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc() const;

    /// <summary>
    /// Bufferの書き込み先のメモリへのポインタを T* として取得します。
    /// </summary>
    template <typename T>
    T *GetPtr()
    {
        return static_cast<T *>(GetPtr());
    }
};