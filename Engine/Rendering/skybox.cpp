#include "pch.h"
#include "skybox.h"
#include "gui.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
Skybox::Skybox()
{
    std::array<Vertex, 8> cube_vertices;
    cube_vertices[0] = {{-1.0f, 1.0f, -1.0f}};
    cube_vertices[1] = {{1.0f, 1.0f, -1.0f}};
    cube_vertices[2] = {{1.0f, -1.0f, -1.0f}};
    cube_vertices[3] = {{-1.0f, -1.0f, -1.0f}};
    cube_vertices[4] = {{-1.0f, 1.0f, 1.0f}};
    cube_vertices[5] = {{1.0f, 1.0f, 1.0f}};
    cube_vertices[6] = {{1.0f, -1.0f, 1.0f}};
    cube_vertices[7] = {{-1.0f, -1.0f, 1.0f}};
    m_vertex_buffer_ = std::make_shared<VertexBuffer>(cube_vertices.size(), cube_vertices.data());

    std::array<uint32_t, 36> cube_indices =
    {
        0, 1, 2, 0, 2, 3,
        5, 4, 7, 5, 7, 6,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
    };

    m_index_buffer_ = std::make_shared<IndexBuffer>(sizeof(uint32_t) * cube_indices.size(), cube_indices.data());

    m_texture_cube_ = std::make_shared<class TextureCube>();
}

bool Skybox::ReconstructTextureCube()
{
    if (m_texture_cube_ == nullptr || !m_texture_cube_->IsValid())
        return false;

    m_texture_cube_handle_ = m_texture_cube_->UploadBuffer();
    return true;
}

std::shared_ptr<Skybox> Skybox::Instance()
{
    static std::shared_ptr<Skybox> instance(new Skybox());
    return instance;
}

void Skybox::Render()
{
    if (m_texture_cube_ == nullptr || !m_texture_cube_->IsValid())
        return;

    if (m_texture_cube_handle_ == nullptr)
        ReconstructTextureCube();

    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, m_vertex_buffer_->View());
    cmd_list->IASetIndexBuffer(m_index_buffer_->View());

    cmd_list->SetPipelineState(PSOManager::Get("Skybox"));
    cmd_list->SetGraphicsRootDescriptorTable(kPixelSRV, m_texture_cube_handle_->HandleGPU);

    cmd_list->DrawIndexedInstanced(36, 1, 0, 0, 0);
}

std::shared_ptr<TextureCube> Skybox::TextureCube() const
{
    return m_texture_cube_;
}

void Skybox::SetTextureCube(const std::shared_ptr<class TextureCube> &texture_cube)
{
    m_texture_cube_ = texture_cube;
    ReconstructTextureCube();
}
}