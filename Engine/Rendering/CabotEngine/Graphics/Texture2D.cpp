#include "pch.h"

#include "Texture2D.h"

#include <stb_image.h>

#include "descriptor_heap.h"
#include <assimp/texture.h>
#include "RenderEngine.h"

#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;

std::shared_ptr<Texture2D> Texture2D::LoadFromAiTexture(const aiTexture *ai_texture)
{
    auto result_texture = Instantiate<Texture2D>();
    unsigned char *pixels = nullptr;
    int width = 0, height = 0, channels = 0;

    if (ai_texture->mHeight == 0)
    {
        pixels = stbi_load_from_memory(
            reinterpret_cast<const unsigned char *>(ai_texture->pcData),
            ai_texture->mWidth,
            &width,
            &height,
            &channels,
            4
        );
        result_texture->tex_data.reserve(width * height * sizeof(PackedVector::XMCOLOR));
    }
    else
    {
        // RGBAのRAWデータ
        width = ai_texture->mWidth;
        height = ai_texture->mHeight;
        channels = 4;
        pixels = reinterpret_cast<unsigned char *>(ai_texture->pcData);
    }
    result_texture->width = width;
    result_texture->height = height;
    result_texture->format = DXGI_FORMAT_R8G8B8A8_UNORM;
    result_texture->tex_data.resize(width * height);
    memcpy(result_texture->tex_data.data(), pixels, width * height * sizeof(PackedVector::XMCOLOR));

    if (pixels)
    {
        stbi_image_free(pixels);
    }

    return result_texture;
}

void Texture2D::OnInspectorGui()
{
    ImGui::Text("Texture2D");
    ImGui::Text("Width: %d", width);
    ImGui::Text("Height: %d", height);
    ImGui::Text("Mip Level: %d", mip_level);
}

void Texture2D::CreateBuffer()
{
    const auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
        format,
        width,
        height,
        1,
        mip_level
    );

    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_p_resource_)
    );

    if (FAILED(hr))
    {
        return;
    }

    m_p_resource_->SetName(L"Texture");

    const D3D12_BOX dest_region = {0, 0, 0, width, height, 1};
    hr = m_p_resource_->WriteToSubresource(
        0,
        &dest_region, // copy all
        tex_data.data(), // origin data addr
        width * sizeof(PackedVector::XMCOLOR), // 1 line size
        width * height * sizeof(PackedVector::XMCOLOR) // all line sizes
    );

    if (FAILED(hr))
    {
        m_p_resource_ = nullptr;
    }
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
    return m_p_resource_ != nullptr;
}

ID3D12Resource *Texture2D::Resource()
{
    if (!IsValid())
    {
        CreateBuffer();
    }

    return m_p_resource_ != nullptr ? m_p_resource_.Get() : nullptr;
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

CEREAL_REGISTER_TYPE(Texture2D)