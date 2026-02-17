#include "pch.h"

#include "billboard_renderer.h"
#include "game_object.h"
#include "Asset/asset_database.h"
#include "Rendering/primitives.h"
#include "Rendering/render_pipeline.h"

namespace engine
{

void BillboardRenderer::UpdateWorldBuffer()
{
    for (auto &world_matrix_buffer : m_world_matrix_buffers_)
    {
        if (!world_matrix_buffer)
        {
            world_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix));
            world_matrix_buffer->CreateBuffer();
        }
    }

    auto world_matrix = GameObject()->Transform()->WorldMatrix();
    
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto &world_matrix_buffer = m_world_matrix_buffers_[current_buffer_idx];
    const auto ptr = world_matrix_buffer->GetPtr<Matrix>();
    *ptr = Billboard::CalcMatrix(world_matrix);
}

void BillboardRenderer::OnConstructed()
{
    shared_material = AssetPtr<Material>::FromInstance(Instantiate<Material>());
    const auto asset_ptr = AssetDatabase::GetAsset("BillboardShader.hlsl");
    shared_material.CastedLock()->shader = AssetPtr<Shader>::FromIAssetPtr(asset_ptr);
    DirectX::BoundingBox::CreateFromPoints(bounds, Vector3(0, 0, 0), Vector3(1, 1, 1));
}

void BillboardRenderer::OnInspectorGui()
{
    Renderer::OnInspectorGui();
    shared_material.CastedLock()->OnInspectorGui();
}

void BillboardRenderer::Render()
{
    UpdateWorldBuffer();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();

    std::vector materials = {shared_material};

    RenderPipeline::Submit(Primitives::GetQuadMesh(), materials, GameObject()->Transform()->Position(), m_world_matrix_buffers_[current_buffer_idx]->GetAddress());
}

const Matrix &BillboardRenderer::BoundsOrigin()
{
    return GameObject()->Transform()->WorldMatrix();
}
}

CEREAL_REGISTER_TYPE(engine::BillboardRenderer)