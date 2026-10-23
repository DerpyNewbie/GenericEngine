#pragma once
#include <vector>

#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_common.h"

namespace CRS
{
/// <summary>
/// シェーダーから見えないヒープの1つのスロットです。
/// </summary>
struct CpuDescriptor
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
    // ヒープの中の番号。Freeで返すときに使う
    uint32_t index = UINT32_MAX;

    /// <summary>
    /// スロットを持っているかどうかです。
    /// </summary>
    bool IsValid() const
    {
        return index != UINT32_MAX;
    }
};

/// <summary>
/// シェーダーから見えないDescriptorHeapのスロットを管理します。リソースごとのSRV / RTV / DSV / Samplerの置き場です。
/// 描画のときは、ここからGpuDescriptorRingへコピーして使います。
/// </summary>
class CpuDescriptorAllocator
{
    ComPtr<ID3D12DescriptorHeap> m_heap_;
    uint32_t m_increment_size_ = 0;
    std::vector<uint32_t> m_free_indices_;

public:
    /// <summary>
    /// capacity個のスロットを持つヒープを作ります。
    /// </summary>
    bool Init(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity);
    /// <summary>
    /// 空いているスロットを1つ確保します。空きがない場合はエラーを出し、無効なCpuDescriptorを返します。
    /// </summary>
    CpuDescriptor Allocate();
    /// <summary>
    /// スロットを返し、descriptorを無効にします。GPUがまだ使っているかは確かめないので、遅延破棄の中で呼びます。
    /// </summary>
    void Free(CpuDescriptor &descriptor);
};

/// <summary>
/// シェーダーから見えるヒープの、連続したスロットの範囲です。先頭のCPU / GPUハンドルを持ちます。
/// </summary>
struct GpuDescriptorRange
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = {};
    uint32_t increment_size = 0;

    /// <summary>
    /// 範囲の中のoffset番目のスロットのCPUハンドルです（ここにディスクリプタをコピーします）。
    /// </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE Cpu(const uint32_t offset) const
    {
        return {cpu.ptr + static_cast<SIZE_T>(offset) * increment_size};
    }
};

/// <summary>
/// シェーダーから見えるDescriptorHeapを、フレームごとの領域に分けて先頭から順に貸し出すリングです。
/// 描画のたびに必要な分をコピーして使い、フレームの始めに巻き戻します。
/// <para>ヒープの並び: [ずっと使うスロット][フレーム0の領域][フレーム1の領域]...</para>
/// </summary>
class GpuDescriptorRing
{
    ComPtr<ID3D12DescriptorHeap> m_heap_;
    uint32_t m_increment_size_ = 0;
    uint32_t m_capacity_per_frame_ = 0;
    // 先頭のこの数は、ずっと使い続けるスロット（ImGuiのフォントなど）
    uint32_t m_persistent_count_ = 0;
    std::vector<uint32_t> m_free_persistent_;
    // 今のフレームの領域の先頭と、そこからの使用済みの数
    uint32_t m_frame_begin_ = 0;
    uint32_t m_offset_ = 0;
    // あふれたエラーをフレームに1回だけ出すため
    bool m_overflow_reported_ = false;

    // ヒープのindex番目から始まる範囲を作る
    GpuDescriptorRange MakeRange(uint32_t index) const;

public:
    /// <summary>
    /// persistent_count + capacity_per_frame * kMaxFramesInFlight 個のスロットを持つヒープを作ります。
    /// </summary>
    bool Init(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity_per_frame,
              uint32_t persistent_count = 0);
    /// <summary>
    /// frame_indexのフレームの領域を、先頭から使い直します。そのフレームの前回のGPU処理が終わってから呼びます。
    /// </summary>
    void BeginFrame(uint32_t frame_index);
    /// <summary>
    /// 今のフレームの領域からcount個の連続したスロットを借ります。足りない場合はfalseを返します（返却は不要です）。
    /// </summary>
    bool Allocate(uint32_t count, GpuDescriptorRange &out_range);

    /// <summary>
    /// フレームをまたいで使い続けるスロットを1つ確保します。
    /// </summary>
    bool AllocatePersistent(GpuDescriptorRange &out_range);
    /// <summary>
    /// AllocatePersistentで確保したスロットを返します。
    /// </summary>
    void FreePersistent(const GpuDescriptorRange &range);

    /// <summary>
    /// ヒープそのものです（コマンドリストに設定するときに使います）。
    /// </summary>
    ID3D12DescriptorHeap *Heap() const
    {
        return m_heap_.Get();
    }
};
}
