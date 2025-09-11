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
    auto instance = Instance();
    for (auto texture : instance->textures)
    {
        if (texture.Lock() == nullptr)
        {
            return;
        }
    }

    std::array<std::shared_ptr<Texture2D>, 6> shared_textures;
    for (int i = 0; i < instance->textures.size(); ++i)
        shared_textures[i] = instance->textures[i].CastedLock();
    if (!instance->m_texture_cube_.CreateTexCube(shared_textures))
    {
        instance->m_is_texture_set_ = false;
        return;
    }
    instance->m_texture_cube_handle_ = DescriptorHeap::Register(instance->m_texture_cube_);
    instance->m_is_texture_set_ = true;
}

Skybox *Skybox::Instance()
{
    static auto instance = new Skybox;
    return instance;
}

void Skybox::Initialize()
{
    auto instance = Instance();
    std::array<Vertex, 8> cube_vertices;
    cube_vertices[0] = {{-1.0f, 1.0f, -1.0f}};
    cube_vertices[1] = {{1.0f, 1.0f, -1.0f}};
    cube_vertices[2] = {{1.0f, -1.0f, -1.0f}};
    cube_vertices[3] = {{-1.0f, -1.0f, -1.0f}};
    cube_vertices[4] = {{-1.0f, 1.0f, 1.0f}};
    cube_vertices[5] = {{1.0f, 1.0f, 1.0f}};
    cube_vertices[6] = {{1.0f, -1.0f, 1.0f}};
    cube_vertices[7] = {{-1.0f, -1.0f, 1.0f}};
    instance->m_vertex_buffer_ = std::make_shared<VertexBuffer>(cube_vertices.size(), cube_vertices.data());

    std::array<uint32_t, 36> cube_indices =
    {
        0, 1, 2, 0, 2, 3,
        5, 4, 7, 5, 7, 6,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
    };
    instance->m_index_buffer_ = std::make_shared<IndexBuffer>(sizeof(uint32_t) * cube_indices.size(),
                                                              cube_indices.data());
}

void Skybox::OnInspectorGui()
{
    const auto &instance = Instance();
    for (int i = 0; i < instance->textures.size(); ++i)
    {
        ImGui::PushID(i);
        if (Gui::PropertyField("Texture", instance->textures[i]))
        {
            if (instance->textures[i].CastedLock() != nullptr)
            {
                instance->textures[i].CastedLock()->CreateBuffer();
                ReConstructTexCube();
            }
        }
        ImGui::PopID();
    }
}

void Skybox::Render()
{
    auto instance = Instance();
    if (!instance->m_is_texture_set_)
        return;

    auto cmd_list = RenderEngine::CommandList();
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, instance->m_vertex_buffer_->View());
    cmd_list->IASetIndexBuffer(instance->m_index_buffer_->View());

    cmd_list->SetPipelineState(PSOManager::Get("Skybox"));
    cmd_list->SetGraphicsRootDescriptorTable(kPixelSRV, instance->m_texture_cube_handle_->HandleGPU);

    cmd_list->DrawIndexedInstanced(36, 1, 0, 0, 0);
}
}