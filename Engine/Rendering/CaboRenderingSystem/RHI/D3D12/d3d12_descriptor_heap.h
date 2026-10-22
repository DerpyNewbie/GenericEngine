#pragma once
#include <vector>

#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_common.h"

namespace CRS
{
struct CpuDescriptor
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
    uint32_t index = UINT32_MAX;

    bool IsValid() const
    {
        return index != UINT32_MAX;
    }
};

/// <summary>
/// シェーダーから見えないDescriptorHeapのスロットを管理します。リソースごとのSRV / RTV / DSV / Samplerの置き場です。
/// </summary>
class CpuDescriptorAllocator
{
    ComPtr<ID3D12DescriptorHeap> m_heap_;
    uint32_t m_increment_size_ = 0;
    std::vector<uint32_t> m_free_indices_;

public:
    bool Init(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity);
    CpuDescriptor Allocate();
    void Free(CpuDescriptor &descriptor);
};

struct GpuDescriptorRange
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = {};
    uint32_t increment_size = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE Cpu(const uint32_t offset) const
    {
        return {cpu.ptr + static_cast<SIZE_T>(offset) * increment_size};
    }
};

/// <summary>
/// シェーダーから見えるDescriptorHeapを、フレームごとの領域に分けて先頭から順に貸し出すリングです。
/// 描画のたびに必要な分をコピーして使い、フレームの始めに巻き戻します。
/// </summary>
class GpuDescriptorRing
{
    ComPtr<ID3D12DescriptorHeap> m_heap_;
    uint32_t m_increment_size_ = 0;
    uint32_t m_capacity_per_frame_ = 0;
    // 先頭のこの数は、ずっと使い続けるスロット（ImGuiのフォントなど）
    uint32_t m_persistent_count_ = 0;
    std::vector<uint32_t> m_free_persistent_;
    uint32_t m_frame_begin_ = 0;
    uint32_t m_offset_ = 0;
    bool m_overflow_reported_ = false;

    GpuDescriptorRange MakeRange(uint32_t index) const;

public:
    bool Init(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity_per_frame,
              uint32_t persistent_count = 0);
    void BeginFrame(uint32_t frame_index);
    bool Allocate(uint32_t count, GpuDescriptorRange &out_range);

    /// <summary>
    /// フレームをまたいで使い続けるスロットを1つ確保します。
    /// </summary>
    bool AllocatePersistent(GpuDescriptorRange &out_range);
    void FreePersistent(const GpuDescriptorRange &range);

    ID3D12DescriptorHeap *Heap() const
    {
        return m_heap_.Get();
    }
};
}