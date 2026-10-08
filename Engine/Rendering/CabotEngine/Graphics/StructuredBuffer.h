#pragma once
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"
#include "Rendering/ibuffer.h"
#include "Rendering/shader_resource.h"

namespace engine
{
/// <summary>
/// 構造体の配列をShaderに渡すためのBufferです。
/// </summary>
class StructuredBuffer final : public IBuffer, public ShaderResource
{
    ComPtr<ID3D12Resource> m_default_buffer_;
    ComPtr<ID3D12Resource> m_upload_buffer_;
    D3D12_GPU_VIRTUAL_ADDRESS m_gpu_address_;
    size_t m_element_count_ = 0;
    size_t m_stride_ = 0;
    
public:
    /// <summary>
    /// 要素のサイズと数を指定してStructuredBufferを準備します。
    /// </summary>
    /// <param name="stride">1要素のサイズ(byte)</param>
    /// <param name="elem_count">要素の数</param>
    explicit StructuredBuffer(const size_t stride, const size_t elem_count)
    {
        m_stride_ = stride;
        m_element_count_ = elem_count;
        m_gpu_address_ = 0;
    }

    /// <summary>
    /// Buffer本体と、書き込みに使うUpload用のBufferを作成します。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// データをUpload用のBufferに書き込み、CommandListでBuffer本体にコピーします。
    /// </summary>
    /// <param name="data">書き込むデータ</param>
    void UpdateBuffer(void *data) override;
    /// <summary>
    /// ShaderResourceViewをDescriptorHeapに登録します。
    /// </summary>
    /// <returns>登録されたDescriptorHandle</returns>
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;

    /// <summary>
    /// 常に true を返します。
    /// </summary>
    bool CanUpdate() override
    {
        return true;
    }

    /// <summary>
    /// Bufferが作成済みであるかどうかを取得します。
    /// </summary>
    bool IsValid() override;

    /// <summary>
    /// StructuredBufferとして読むためのSRVの設定を取得します。
    /// </summary>
    [[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
    /// <summary>
    /// Buffer本体のResourceを取得します。
    /// </summary>
    [[nodiscard]] ID3D12Resource *Resource() override;

    /// <summary>
    /// Buffer本体のGPU上のアドレスを取得します。
    /// </summary>
    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

};
}