#include "pch.h"
#include "skybox.h"
#include "camera.h"
#include "gui.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
void Skybox::ReConstructTexCube()
{
    for (auto texture : textures)
    {
        if (texture.Lock() == nullptr)
        {
            return;
        }
    }

    std::array<std::shared_ptr<Texture2D>, 6> shared_textures;
    for (int i = 0; i < textures.size(); ++i)
        shared_textures[i] = textures[i].CastedLock();
    if (!m_texture_cube_.CreateTexCube(shared_textures))
    {
        m_is_texture_set_ = false;
        return;
    }
    m_texture_cube_handle_ = DescriptorHeap::Register(m_texture_cube_);
    m_is_texture_set_ = true;
}

void Skybox::OnConstructed()
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
}

void Skybox::OnInspectorGui()
{
    for (int i = 0; i < textures.size(); ++i)
    {
        ImGui::PushID(i);
        if (Gui::PropertyField("Texture", textures[i]))
        {
            textures[i].CastedLock()->CreateBuffer();
            ReConstructTexCube();
        }
        ImGui::PopID();
    }
}

void Skybox::OnDraw()
{
    if (!m_is_texture_set_)
        return;

    auto cmd_list = g_RenderEngine->CommandList();
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, m_vertex_buffer_->View());
    cmd_list->IASetIndexBuffer(m_index_buffer_->View());

    cmd_list->SetPipelineState(PSOManager::Get("Skybox"));
    cmd_list->SetGraphicsRootDescriptorTable(kPixelSRV, m_texture_cube_handle_->HandleGPU);

    cmd_list->DrawIndexedInstanced(36, 1, 0, 0, 0);
}

std::shared_ptr<Transform> Skybox::BoundsOrigin()
{
    return Camera::Main()->GameObject()->Transform();
}
}