#include "pch.h"
#include "renderer_2d.h"
#include "image.h"
#include "rect_transform.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

using namespace DirectX::SimpleMath;

namespace engine
{
void Image::OnInspectorGui()
{
    Gui::PropertyField("Material", m_material_);

    if (auto material = m_material_.CastedLock())
    {
        material->OnInspectorGui();
    }
}

void Image::OnAwake()
{
    Renderer2D::OnAwake();
    //create index buffer
    if (!m_index_buffer_)
    {
        std::vector<uint32_t> indices =
        {
            0, 1, 2, 2, 1, 3
        };
        m_index_buffer_ = std::make_shared<IndexBuffer>(indices.size() * sizeof(uint32_t), indices.data());
        if (!m_index_buffer_->IsValid())
        {
            Logger::Error<Renderer2D>("Failed to create IndexBuffer!");
        }
    }
}

void Image::OnUpdate()
{
    Renderer2D::OnUpdate();
    if (auto rect_transform = GameObject()->GetComponent<RectTransform>())
    {
        auto rect = NormalizedRect();
        auto min_pos = rect.pos - rect.size / 2;
        auto max_pos = rect.pos + rect.size / 2;
        std::array<Vertex, 4> vertices = {};
        vertices[0].vertex = Vector3(min_pos.x, min_pos.y, 0.0f);
        vertices[1].vertex = Vector3(min_pos.x, max_pos.y, 0.0f);
        vertices[2].vertex = Vector3(max_pos.x, min_pos.y, 0.0f);
        vertices[3].vertex = Vector3(max_pos.x, max_pos.y, 0.0f);
        vertices[0].uvs[0] = Vector2(0, 1);
        vertices[1].uvs[0] = Vector2(0, 0);
        vertices[2].uvs[0] = Vector2(1, 1);
        vertices[3].uvs[0] = Vector2(1, 0);

        m_vertex_buffer_[RenderEngine::CurrentBackBufferIndex()] = std::make_shared<VertexBuffer>(
            vertices.size(), vertices.data());
    }
}

void Image::Render()
{
    const auto current_buffer = RenderEngine::CurrentBackBufferIndex();
    if (m_vertex_buffer_[current_buffer] == nullptr)
        return;

    const auto material = m_material_.CastedLock();
    if (material == nullptr)
        return;

    const auto shader = material->p_shared_shader.CastedLock();
    const auto cmd_list = RenderEngine::CommandList();

    if (material->IsValid())
    {
        if (shader)
        {
            PSOManager::SetPipelineState(cmd_list, shader);
        }

        material->SetDescriptorTable();
        cmd_list->IASetIndexBuffer(m_index_buffer_->View());
        cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd_list->IASetVertexBuffers(0, 1, m_vertex_buffer_[current_buffer]->View());

        cmd_list->DrawIndexedInstanced(6, 1, 0, 0, 0);
    }
}

AssetPtr<Texture2D> Image::GetTexture()
{
    return m_texture_;
}

void Image::SetTexture(AssetPtr<Texture2D> texture)
{
    if (auto tex = texture.CastedLock())
    {
        m_texture_ = texture;
        tex->CreateBuffer();
        if (m_texture_handle_ != nullptr)
            DescriptorHeap::Free(m_texture_handle_);
        m_texture_handle_ = tex->UploadBuffer();
    }
}
}

CEREAL_REGISTER_TYPE(engine::Image)