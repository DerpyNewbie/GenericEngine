#pragma once
#include <d3d12.h>
#include "DescriptorHeap.h"
#include "ComPtr.h"
#include "Material.h"
#include "StructuredBuffer.h"

class MaterialBuffer
{
public:
    std::shared_ptr<DescriptorHandle> Handle;
    
    MaterialBuffer() = default;

    void Initialize(Material& material);
    
private:
    StructuredBuffer<float> m_ValuesBuffer;
    
};
