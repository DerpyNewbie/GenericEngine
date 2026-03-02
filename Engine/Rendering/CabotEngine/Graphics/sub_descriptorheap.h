#pragma once
#include <directx/d3d12.h>
#include "DescriptorHeap.h"

class SubDescriptorHeap
{
    ID3D12DescriptorHeap *m_heap_;
    uint32_t m_start_index_;
    uint32_t m_capacity_;
    uint32_t m_current_offset_;
    uint32_t m_descriptor_size_;

public:
    SubDescriptorHeap(ID3D12DescriptorHeap *heap, uint32_t start_index, uint32_t capacity, uint32_t descriptor_size);
    DescriptorHandle Allocate(uint32_t count = 1);
    void Reset();

};