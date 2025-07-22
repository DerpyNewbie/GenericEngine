#include "billboard_renderer.h"
#include "game_object.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

void engine::BillboardRenderer::SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list)
{
    auto material_handles = shared_material->p_shared_material_block->GetHandles();
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
            if (!material_handles[shader_type][params_type].empty())
                cmd_list->SetGraphicsRootDescriptorTable(
                    shader_type * 3 + params_type + 2,
                    material_handles[shader_type][params_type][0]->HandleGPU);
}

void engine::BillboardRenderer::OnConstructed()
{
    shared_material = Instantiate<Material>();
    DirectX::BoundingBox::CreateFromPoints(bounds, Vector3(0, 0, 0), Vector3(1, 1, 1));
}

void engine::BillboardRenderer::OnInspectorGui()
{
    shared_material->OnInspectorGui();
}

void engine::BillboardRenderer::OnDraw()
{
    m_billboard_.world_matrix = GameObject()->Transform()->WorldMatrix();
    m_billboard_.Update();

    if (shared_material->IsValid() && !shared_material->p_shared_material_block->IsCreateBuffer())
    {
        shared_material->p_shared_material_block->CreateBuffer();
    }

    if (shared_material->IsValid())
    {
        auto shader = shared_material->p_shared_shader.CastedLock();
        const auto cmd_list = g_RenderEngine->CommandList();
        const auto vb_view = m_billboard_.vertex_buffer->View();
        const auto ibView = m_billboard_.index_buffer->View();
        auto current_buffer = g_RenderEngine->CurrentBackBufferIndex();

        g_PSOManager.SetPipelineState(cmd_list, shader);
        cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd_list->IASetVertexBuffers(0, 1, &vb_view);
        cmd_list->IASetIndexBuffer(&ibView);
        cmd_list->SetGraphicsRootConstantBufferView(kWVPCBV,
                                                    m_billboard_.wvp_buffers[current_buffer]->GetAddress());
        SetDescriptorTable(cmd_list);

        cmd_list->DrawIndexedInstanced(6, 1, 0, 0, 0);
    }
}

std::weak_ptr<engine::Transform> engine::BillboardRenderer::GetTransform()
{
    return GameObject()->Transform();
}

CEREAL_REGISTER_TYPE(engine::BillboardRenderer)