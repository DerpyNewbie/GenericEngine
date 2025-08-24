#include "sky_box.h"
#include "camera.h"
#include "gui.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

void engine::SkyBox::OnConstructed()
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
    m_VertexBuffer_ = std::make_shared<VertexBuffer>(cube_vertices.size(), cube_vertices.data());

    std::array<uint32_t, 36> cube_indices =
    {
        0, 1, 2, 0, 2, 3,
        5, 4, 7, 5, 7, 6,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
    };
    m_IndexBuffer_ = std::make_shared<IndexBuffer>(sizeof(uint32_t) * cube_indices.size(), cube_indices.data());
    for (auto &vp_buffer : viewproj_buffers)
    {
        vp_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix));
        vp_buffer->CreateBuffer();
    }
}

void engine::SkyBox::OnInspectorGui()
{
    bool changed = false;
    for (int i = 0; i < textures.size(); ++i)
    {
        ImGui::PushID(i);
        if (Gui::PropertyField("Texture", textures[i]))
        {
            textures[i].CastedLock()->CreateBuffer();
            changed = true;
        }
        ImGui::PopID();
    }
    if (changed)
    {
        bool can_create = true;
        for (auto texture : textures)
        {
            if (!texture.Lock())
            {
                can_create = false;
            }
        }

        if (can_create)
        {
            m_IsTextureSet = false;
            std::array<std::shared_ptr<Texture2D>, 6> shared_textures;
            for (int i = 0; i < textures.size(); ++i)
                shared_textures[i] = textures[i].CastedLock();
            if (m_TextureCube_.CreateTexCube(shared_textures))
            {
                m_TextureCubeHandle_ = DescriptorHeap::Register(m_TextureCube_);
                m_IsTextureSet = true;
            }
        }
    }
}

void engine::SkyBox::OnDraw()
{
    if (!m_IsTextureSet)
    {
        return;
    }

    auto cmd_list = g_RenderEngine->CommandList();
    auto current_buffer = g_RenderEngine->CurrentBackBufferIndex();
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, m_VertexBuffer_->View());
    cmd_list->IASetIndexBuffer(m_IndexBuffer_->View());

    cmd_list->SetPipelineState(PSOManager::Get("SkyBox"));
    cmd_list->SetGraphicsRootDescriptorTable(kPixelSRV, m_TextureCubeHandle_->HandleGPU);

    cmd_list->DrawIndexedInstanced(36, 1, 0, 0, 0);
}
std::shared_ptr<engine::Transform> engine::SkyBox::BoundsOrigin()
{
    return Camera::Main()->GameObject()->Transform();
}