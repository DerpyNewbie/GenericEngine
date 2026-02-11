#include "pch.h"
#include "renderer_2d.h"
#include "image.h"
#include "rect_transform.h"
#include "Rendering/quad_mesh.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

using namespace DirectX::SimpleMath;

namespace engine
{
void Image::UpdateWorldBuffer()
{
    for (auto &world_matrix_buffer : m_world_matrix_buffers_)
    {
        if (!world_matrix_buffer)
        {
            world_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix));
            world_matrix_buffer->CreateBuffer();
        }
    }

    auto rect = NormalizedRect();
    if (auto rect_transform = GameObject()->GetComponent<RectTransform>())
    {
        rect = NormalizedRect();
        rect.pos - rect.size / 2;
        rect.pos + rect.size / 2;
    }

    const auto scale_mat = Matrix::CreateScale(rect.size.x, rect.size.y, 1.0f);
    const auto trans_mat = Matrix::CreateTranslation(rect.pos.x, rect.pos.y, 0.0f);

    const auto world_mat = scale_mat * trans_mat;
    
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto &world_matrix_buffer = m_world_matrix_buffers_[current_buffer_idx];
    const auto ptr = world_matrix_buffer->GetPtr<Matrix>();
    *ptr = world_mat;
}

void Image::OnInspectorGui()
{
    Gui::ExpandablePropertyField("Material", shared_material);
}

void Image::OnUpdate()
{
    Renderer2D::OnUpdate();
}

void Image::Render()
{
    UpdateWorldBuffer();
    
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    std::vector materials = {shared_material};
    
    RenderPipeline::Submit(QuadMesh::GetMesh(), materials, Vector3::Zero, m_world_matrix_buffers_[current_buffer_idx]->GetAddress());
}
}

CEREAL_REGISTER_TYPE(engine::Image)