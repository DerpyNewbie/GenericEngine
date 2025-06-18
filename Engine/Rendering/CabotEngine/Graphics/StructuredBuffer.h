#pragma once
#include <d3d12.h>
#include <vector>
#include "ComPtr.h"
#include "RenderEngine.h"

template <typename T>
class StructuredBuffer
{
public:
    StructuredBuffer() = default;
    
    void Initialize(size_t elementCount)
    {
        m_elementCount = elementCount;
        size_t totalSize = sizeof(T) * elementCount;

        // GPU側リソース
        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);

        auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        
        auto heap_prop =CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

        // アップロード用バッファ
        g_RenderEngine->Device()->CreateCommittedResource(
            &heap_prop,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_pResource)
        );
    }
    
    void Upload(const std::vector<T>& data)
    {
        void* mapped = nullptr;
        m_pResource->Map(0, nullptr, &mapped);
        memcpy(mapped, data.data(), sizeof(T) * data.size());
        m_pResource->Unmap(0, nullptr);
    }
    
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc()
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = static_cast<UINT>(m_elementCount);
        srvDesc.Buffer.StructureByteStride = sizeof(T);
        srvDesc.Buffer.FirstElement = 0;
        return srvDesc;
    }

    ID3D12Resource* Resource(){return  m_pResource.Get();}
    
private:
    ComPtr<ID3D12Resource> m_pResource;
    size_t m_elementCount = 0;
};
