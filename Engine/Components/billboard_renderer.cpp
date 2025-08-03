#include "billboard_renderer.h"
#include "game_object.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

void engine::BillboardRenderer::SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list)
{
    const auto material = shared_material;
    const auto material_block = material->p_shared_material_block;

    material->UpdateBuffer();

    for (int shader_i = 0; shader_i < kShaderType_Count; ++shader_i)
    {
        for (int param_i = 0; param_i < kParameterBufferType_Count; ++param_i)
        {
            const auto shader_type = static_cast<kShaderType>(shader_i);
            const auto param_type = static_cast<kParameterBufferType>(param_i);

            if (material_block->Empty(shader_type, param_type))
            {
                continue;
            }

            // +2 for engine pre-defined shader variables
            const int root_param_idx = shader_type * kParameterBufferType_Count + param_i + 2;
            const auto itr = material_block->Begin(shader_type, param_type);
            const auto desc_handle = itr->handle->HandleGPU;
            cmd_list->SetGraphicsRootDescriptorTable(root_param_idx, desc_handle);
        }
    }
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

    shared_material->p_shared_material_block->UpdateBuffer();

    if (shared_material->IsValid())
    {
        auto shader = shared_material->p_shared_shader.CastedLock();
        const auto cmd_list = g_RenderEngine->CommandList();
        const auto vb_view = m_billboard_.vertex_buffer->View();
        const auto ibView = m_billboard_.index_buffer->View();
        auto current_buffer = g_RenderEngine->CurrentBackBufferIndex();

        PSOManager::SetPipelineState(cmd_list, shader);
        cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd_list->IASetVertexBuffers(0, 1, &vb_view);
        cmd_list->IASetIndexBuffer(&ibView);
        cmd_list->SetGraphicsRootConstantBufferView(kWVPCBV,
                                                    m_billboard_.wvp_buffers[current_buffer]->GetAddress());
        SetDescriptorTable(cmd_list);

        cmd_list->DrawIndexedInstanced(6, 1, 0, 0, 0);
    }
}

std::weak_ptr<engine::Transform> engine::BillboardRenderer::BoundsOrigin()
{
    return GameObject()->Transform();
}

CEREAL_REGISTER_TYPE(engine::BillboardRenderer)