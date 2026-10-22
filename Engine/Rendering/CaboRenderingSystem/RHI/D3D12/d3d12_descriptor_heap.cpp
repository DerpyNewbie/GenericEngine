#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_descriptor_heap.h"

#include "Rendering/CaboRenderingSystem/Core/log.h"

namespace CRS
{
bool CpuDescriptorAllocator::Init(ID3D12Device *device, const D3D12_DESCRIPTOR_HEAP_TYPE type,
                                  const uint32_t capacity)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = type;
    desc.NumDescriptors = capacity;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap_))))
        return false;

    m_increment_size_ = device->GetDescriptorHandleIncrementSize(type);
    m_free_indices_.resize(capacity);
    for (uint32_t i = 0; i < capacity; ++i)
        m_free_indices_[i] = capacity - 1 - i;

    return true;
}

CpuDescriptor CpuDescriptorAllocator::Allocate()
{
    if (m_free_indices_.empty())
    {
        LogError("CPU descriptor heap is full");
        return {};
    }

    const auto index = m_free_indices_.back();
    m_free_indices_.pop_back();

    CpuDescriptor descriptor;
    descriptor.index = index;
    descriptor.handle.ptr = m_heap_->GetCPUDescriptorHandleForHeapStart().ptr +
                            static_cast<SIZE_T>(index) * m_increment_size_;
    return descriptor;
}

void CpuDescriptorAllocator::Free(CpuDescriptor &descriptor)
{
    if (!descriptor.IsValid())
        return;

    m_free_indices_.push_back(descriptor.index);
    descriptor = {};
}

bool GpuDescriptorRing::Init(ID3D12Device *device, const D3D12_DESCRIPTOR_HEAP_TYPE type,
                             const uint32_t capacity_per_frame, const uint32_t persistent_count)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = type;
    desc.NumDescriptors = persistent_count + capacity_per_frame * kMaxFramesInFlight;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap_))))
        return false;

    m_increment_size_ = device->GetDescriptorHandleIncrementSize(type);
    m_capacity_per_frame_ = capacity_per_frame;
    m_persistent_count_ = persistent_count;
    m_free_persistent_.resize(persistent_count);
    for (uint32_t i = 0; i < persistent_count; ++i)
        m_free_persistent_[i] = persistent_count - 1 - i;
    return true;
}

GpuDescriptorRange GpuDescriptorRing::MakeRange(const uint32_t index) const
{
    GpuDescriptorRange range;
    range.increment_size = m_increment_size_;
    range.cpu.ptr = m_heap_->GetCPUDescriptorHandleForHeapStart().ptr + static_cast<SIZE_T>(index) * m_increment_size_;
    range.gpu.ptr = m_heap_->GetGPUDescriptorHandleForHeapStart().ptr + static_cast<UINT64>(index) * m_increment_size_;
    return range;
}

void GpuDescriptorRing::BeginFrame(const uint32_t frame_index)
{
    m_frame_begin_ = m_persistent_count_ + frame_index * m_capacity_per_frame_;
    m_offset_ = 0;
    m_overflow_reported_ = false;
}

bool GpuDescriptorRing::Allocate(const uint32_t count, GpuDescriptorRange &out_range)
{
    if (m_offset_ + count > m_capacity_per_frame_)
    {
        if (!m_overflow_reported_)
        {
            LogError("GPU descriptor ring overflow ({} per frame)", m_capacity_per_frame_);
            m_overflow_reported_ = true;
        }
        return false;
    }

    out_range = MakeRange(m_frame_begin_ + m_offset_);
    m_offset_ += count;
    return true;
}

bool GpuDescriptorRing::AllocatePersistent(GpuDescriptorRange &out_range)
{
    if (m_free_persistent_.empty())
    {
        LogError("No persistent GPU descriptor left");
        return false;
    }

    out_range = MakeRange(m_free_persistent_.back());
    m_free_persistent_.pop_back();
    return true;
}

void GpuDescriptorRing::FreePersistent(const GpuDescriptorRange &range)
{
    const auto index = static_cast<uint32_t>(
        (range.gpu.ptr - m_heap_->GetGPUDescriptorHandleForHeapStart().ptr) / m_increment_size_);
    if (index < m_persistent_count_)
        m_free_persistent_.push_back(index);
}
}