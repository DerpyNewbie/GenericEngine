#pragma once
#include "TextureCube.h"

namespace engine
{
class ConstantBuffer;
class ShaderResource;
class MaterialBlock;
class StructuredBuffer;


/// <summary>
/// DescriptorHeap上の1つのスロットを表すハンドルです。CPU/GPUハンドルとHeap内のインデックスを持ちます。
/// </summary>
class DescriptorHandle
{
public:
    D3D12_CPU_DESCRIPTOR_HANDLE handle_cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE handle_gpu;
    UINT index;
};

/// <summary>
/// CBV/SRV/UAV用のShaderVisibleなDescriptorHeapを管理するシングルトンクラスです。スロットの確保と解放を行います。
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
    /// 最大数分のDescriptorHeapを作成し、すべてのスロットを空きとして登録します。
    /// </summary>
    DescriptorHeap();

    /// <summary>
    /// DescriptorHeapを取得します。
    /// </summary>
    static ID3D12DescriptorHeap *GetHeap();
    /// <summary>
    /// スロットを確保し、ShaderResourceのSRVを作成します。
    /// </summary>
    /// <param name="shader_resource">登録するShaderResource</param>
    /// <returns>確保したハンドル</returns>
    static std::shared_ptr<DescriptorHandle> Register(engine::ShaderResource *shader_resource);
    /// <summary>
    /// スロットを確保し、ConstantBufferのCBVを作成します。
    /// </summary>
    /// <param name="constant_buffer">登録するConstantBuffer</param>
    /// <returns>確保したハンドル</returns>
    static std::shared_ptr<DescriptorHandle> Register(const ConstantBuffer &constant_buffer);

    /// <summary>
    /// 空いているスロットを1つ確保します。
    /// </summary>
    /// <returns>確保したハンドル。空きがない場合はnullptr</returns>
    static std::shared_ptr<DescriptorHandle> Allocate();
    /// <summary>
    /// 指定したインデックスのスロットを確保します。
    /// </summary>
    /// <param name="index">確保するスロットのインデックス</param>
    /// <returns>確保したハンドル。範囲外または使用中の場合はnullptr</returns>
    static std::shared_ptr<DescriptorHandle> Allocate(uint32_t index);
    /// <summary>
    /// 連続した空きスロットを指定した数だけ確保します。DescriptorTableとしてまとめて渡す場合に使用します。
    /// </summary>
    /// <param name="count">確保する数</param>
    /// <returns>確保したハンドルの配列。確保できなかった場合は空</returns>
    static std::vector<std::shared_ptr<DescriptorHandle>> AllocateLinedUp(size_t count);
    /// <summary>
    /// スロットを解放し、空きとして登録します。
    /// </summary>
    /// <param name="handle">解放するハンドル</param>
    static void Free(const std::shared_ptr<DescriptorHandle> &handle);

    /// <summary>
    /// すべてのハンドルと空きスロットの情報を破棄します。
    /// </summary>
    static void Release();
};
}