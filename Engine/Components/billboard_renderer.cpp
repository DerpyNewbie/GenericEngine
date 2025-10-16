#include "pch.h"

#include "billboard_renderer.h"
#include "game_object.h"
#include "Asset/asset_database.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

namespace engine
{
void BillboardRenderer::SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list)
{
    shared_material->SetDescriptorTable();
}

void BillboardRenderer::OnConstructed()
{
    shared_material = Instantiate<Material>();
    auto asset_ptr = AssetDatabase::GetAsset("BillboardShader.hlsl");
    shared_material->p_shared_shader = AssetPtr<Shader>::FromIAssetPtr(asset_ptr);
    DirectX::BoundingBox::CreateFromPoints(bounds, Vector3(0, 0, 0), Vector3(1, 1, 1));
}

void BillboardRenderer::OnInspectorGui()
{
    shared_material->OnInspectorGui();
}

void BillboardRenderer::UpdateBuffer()
{
    m_billboard_.world_matrix = GameObject()->Transform()->WorldMatrix();
    m_billboard_.Update();

    shared_material->p_shared_material_block->UpdateBuffer();
}

void BillboardRenderer::Render()
{
    if (shared_material->IsValid())
    {
        auto shader = shared_material->p_shared_shader.CastedLock();
        const auto cmd_list = RenderEngine::CommandList();
        auto current_buffer = RenderEngine::CurrentBackBufferIndex();

        PSOManager::SetPipelineState(cmd_list, shader);
        cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd_list->IASetVertexBuffers(0, 1, m_billboard_.vertex_buffer->View());
        cmd_list->IASetIndexBuffer(m_billboard_.index_buffer->View());
        cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV,
                                                    m_billboard_.wvp_buffers[current_buffer]->GetAddress());
        SetDescriptorTable(cmd_list);

        cmd_list->DrawIndexedInstanced(6, 1, 0, 0, 0);
    }
}

std::shared_ptr<Transform> BillboardRenderer::BoundsOrigin()
{
    return GameObject()->Transform();
}
}

CEREAL_REGISTER_TYPE(engine::BillboardRenderer)