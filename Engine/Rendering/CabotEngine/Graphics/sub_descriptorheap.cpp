#include "pch.h"
#include "sub_descriptorheap.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "RenderEngine.h"

SubDescriptorHeap::SubDescriptorHeap(ID3D12DescriptorHeap *heap, uint32_t start_index, uint32_t capacity, uint32_t descriptor_size) : m_heap_(heap),
    m_start_index_(start_index),
    m_capacity_(capacity),
    m_current_offset_(0),
    m_descriptor_size_(descriptor_size)
{}

DescriptorHandle SubDescriptorHeap::Allocate(const uint32_t count)
{
    DescriptorHandle handle;

    handle.index = m_start_index_ + m_current_offset_;

    handle.HandleCPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_heap_->GetCPUDescriptorHandleForHeapStart(),
        handle.index, m_descriptor_size_);

    handle.HandleGPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(
        m_heap_->GetGPUDescriptorHandleForHeapStart(),
        handle.index, m_descriptor_size_);

    m_current_offset_ += count;
    return handle;
}

void SubDescriptorHeap::Reset()
{
    m_current_offset_ = 0;
}