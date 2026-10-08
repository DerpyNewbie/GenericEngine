#pragma once
#include "TextureCube.h"

class ConstantBuffer;
class Texture2D;


namespace engine
{
class ShaderResource;
class MaterialBlock;
class StructuredBuffer;
}

/// <summary>
/// DescriptorHeap上の1つのDescriptorのHandleです。
/// </summary>
class DescriptorHandle
{
public:
    D3D12_CPU_DESCRIPTOR_HANDLE handle_cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE handle_gpu;
    UINT index;
};

/// <summary>
/// ShaderResourceのDescriptorを登録、管理するHeapです。
/// </summary>
class DescriptorHeap
{
    static constexpr uint32_t kHandleMax = 512;
    static std::shared_ptr<DescriptorHeap> m_instance_;

    bool m_is_valid_ = false;
    uint32_t m_increment_size_ = 0;
    std::vector<uint32_t> m_free_indices_;
    ComPtr<ID3D12DescriptorHeap> m_p_heap_ = nullptr;
    std::vector<std::shared_ptr<DescriptorHandle>> m_p_handles_;

    /// <summary>
    /// DescriptorHeapの唯一のインスタンスを取得します。存在しない場合は生成します。
    /// </summary>
    static std::shared_ptr<DescriptorHeap> Instance();

public:
    /// <summary>
    /// CBV / SRV / UAV用の、Shaderから参照できるDescriptorHeapを作成します。
    /// </summary>
    DescriptorHeap();

    /// <summary>
    /// DirectXのDescriptorHeapを取得します。
    /// </summary>
    static ID3D12DescriptorHeap *GetHeap();
    /// <summary>
    /// DescriptorHandleを確保し、ShaderResourceViewを作成します。
    /// </summary>
    /// <param name="shader_resource">登録するResource</param>
    /// <returns>確保されたDescriptorHandle</returns>
    static std::shared_ptr<DescriptorHandle> Register(engine::ShaderResource *shader_resource);
    /// <summary>
    /// DescriptorHandleを確保し、ConstantBufferViewを作成します。
    /// </summary>
    /// <param name="constant_buffer">登録するConstantBuffer</param>
    /// <returns>確保されたDescriptorHandle</returns>
    static std::shared_ptr<DescriptorHandle> Register(ConstantBuffer &constant_buffer);

    /// <summary>
    /// 空いているスロットを1つ確保します。
    /// </summary>
    /// <returns>確保されたDescriptorHandle。空きがない場合 nullptr</returns>
    static std::shared_ptr<DescriptorHandle> Allocate();
    /// <summary>
    /// DescriptorHandleを解放し、スロットを空きに戻します。
    /// </summary>
    /// <param name="handle">解放するDescriptorHandle</param>
    static void Free(std::shared_ptr<DescriptorHandle> handle);

    /// <summary>
    /// すべてのDescriptorHandleと空きスロットの管理情報をクリアします。
    /// </summary>
    static void Release();
};