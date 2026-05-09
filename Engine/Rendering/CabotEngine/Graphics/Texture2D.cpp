#include "pch.h"

#include "Texture2D.h"

#include <stb_image.h>

#include "DescriptorHeap.h"
#include <assimp/texture.h>
#include "RenderEngine.h"

#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;

void Texture2D::LoadFromAiTexture(const aiTexture *ai_texture)
{
    unsigned char *pixels;
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
        m_tex_data_.reserve(width * height * sizeof(PackedVector::XMCOLOR));
    }
    else
    {
        width = ai_texture->mWidth;
        height = ai_texture->mHeight;
        pixels = reinterpret_cast<unsigned char *>(ai_texture->pcData);
    }
    m_width_ = width;
    m_height_ = height;
    m_format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_tex_data_.resize(width * height);
    memcpy(m_tex_data_.data(), pixels, width * height * sizeof(PackedVector::XMCOLOR));

    if (pixels)
    {
        stbi_image_free(pixels);
    }
}

void Texture2D::OnInspectorGui()
{
    ImGui::Text("Texture2D");
    ImGui::Text("Width: %d", m_width_);
    ImGui::Text("Height: %d", m_height_);
    ImGui::Text("Mip Level: %d", m_mip_level_);

    if (const auto desc_heap = UploadBuffer())
    {
        const auto ratio = m_height_ > 0 ? static_cast<float>(m_width_) / static_cast<float>(m_height_) : 1.0f;
        const auto max_width = ImGui::CalcItemWidth();
        static float scale = 1.0f;
        ImGui::SliderFloat("Preview Scale", &scale, 0.1f, 1.0f);
        ImGui::Image(desc_heap->handle_gpu.ptr, ImVec2(scale * max_width, scale * max_width * ratio));
        DescriptorHeap::Free(desc_heap);
    }
    else
    {
        ImGui::Text("Could not preview the texture.");
    }
}

void Texture2D::CreateBuffer()
{
    const auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
        m_format_,
        m_width_,
        m_height_,
        1,
        m_mip_level_
    );

    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_buffer_)
    );

    if (FAILED(hr))
    {
        engine::Logger::Error<Texture2D>("failed to create texture2d resource");
        return;
    }

    m_buffer_->SetName(L"Texture");

    const D3D12_BOX dest_region = {0, 0, 0, m_width_, m_height_, 1};
    hr = m_buffer_->WriteToSubresource(
        0,
        &dest_region,
        m_tex_data_.data(),
        m_width_ * sizeof(PackedVector::XMCOLOR),
        m_width_ * m_height_ * sizeof(PackedVector::XMCOLOR)
    );

    if (FAILED(hr))
    {
        m_buffer_ = nullptr;
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
    return m_buffer_ != nullptr;
}

ID3D12Resource *Texture2D::Resource()
{
    if (!IsValid())
    {
        CreateBuffer();
    }

    return m_buffer_ != nullptr ? m_buffer_.Get() : nullptr;
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture2D::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = Resource()->GetDesc().Format;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipLevels = 1;
    return desc;
}

CEREAL_REGISTER_TYPE(Texture2D)