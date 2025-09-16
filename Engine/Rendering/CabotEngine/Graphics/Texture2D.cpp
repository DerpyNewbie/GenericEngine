#include "pch.h"

#include "Texture2D.h"
#include "DescriptorHeap.h"
#include <assimp/texture.h>
#include "RenderEngine.h"

#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;

void Texture2D::OnInspectorGui()
{
    ImGui::Text("Texture2D");
    ImGui::Text("Width: %d", width);
    ImGui::Text("Height: %d", height);
    ImGui::Text("Mip Level: %d", mip_level);
}

void Texture2D::CreateBuffer()
{
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(format,
                                             width,
                                             height,
                                             1,
                                             mip_level);

    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_pResource)
        );

    if (FAILED(hr))
    {
        return;
    }

    m_pResource->SetName(L"Texture");

    D3D12_BOX destRegion = {0, 0, 0, width, height, 1};
    hr = m_pResource->WriteToSubresource(0,
                                         &destRegion, // copy all
                                         tex_data.data(), // origin data addr
                                         width * sizeof(PackedVector::XMCOLOR), // 1 line size
                                         width * height * sizeof(PackedVector::XMCOLOR) // all line sizes
        );

    if (FAILED(hr))
    {
        return;
    }

    m_IsValid = true;
}

void Texture2D::UpdateBuffer(void *data)
{
    engine::Logger::Error("Can not Update Texture2D");
}

std::shared_ptr<DescriptorHandle> Texture2D::UploadBuffer()
{
    return DescriptorHeap::Register(this);
}

bool Texture2D::CanUpdate()
{
    return false;
}

bool Texture2D::IsValid()
{
    return m_IsValid;
}

ID3D12Resource *Texture2D::Resource()
{
    if (m_pResource == nullptr)
    {
        CreateBuffer();
    }

    return m_pResource != nullptr ? m_pResource.Get() : nullptr;
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture2D::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = Resource()->GetDesc().Format;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D texture
    desc.Texture2D.MipLevels = 1; // no mipmaps
    return desc;
}