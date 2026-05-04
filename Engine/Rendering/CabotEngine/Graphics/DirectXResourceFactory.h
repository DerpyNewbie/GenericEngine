#pragma once
#include "ComPtr.h"

class DirectXResourceFactory
{

public:
    static ComPtr<ID3D12Resource> CreateBuffer(const D3D12_HEAP_PROPERTIES &heap_prop, const D3D12_RESOURCE_DESC &res_desc,
        D3D12_RESOURCE_STATES init_state, D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE, const D3D12_CLEAR_VALUE *clear_value = nullptr);

    static ComPtr<ID3D12Resource> CreateUploadedBuffer(const void *data, size_t size, D3D12_HEAP_FLAGS heap_flags, D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_GENERIC_READ, const D3D12_CLEAR_VALUE *clear_value = nullptr);
};