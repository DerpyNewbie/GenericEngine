#include "pch.h"
#include "renderer_2d.h"
#include "image.h"
#include "rect_transform.h"
#include "Rendering/primitives.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

using namespace DirectX::SimpleMath;

namespace engine
{
void Image::UpdateWorldBuffer()
{
    if (!m_world_matrix_buffer_)
    {
        m_world_matrix_buffer_ = std::make_unique<ConstantBuffer>(sizeof(Matrix));
        m_world_matrix_buffer_->CreateBuffer();
    }

    auto rect = NormalizedRect();
    if (auto rect_transform = GameObject()->GetComponent<RectTransform>())
    {
        rect = NormalizedRect();
    }

    const auto scale_mat = Matrix::CreateScale(rect.size.x, rect.size.y, 1.0f);
    const auto trans_mat = Matrix::CreateTranslation(rect.pos.x, rect.pos.y, 0.0f);

    const auto world_mat = scale_mat * trans_mat;

    const auto &world_matrix_buffer = m_world_matrix_buffer_;
    const auto ptr = world_matrix_buffer->GetPtr<Matrix>();
    *ptr = world_mat;
}

void Image::OnInspectorGui()
{
    Gui::ExpandablePropertyField("Material", shared_material);
}

void Image::Render()
{
    UpdateWorldBuffer();

    std::vector materials = {shared_material};

    RenderPipeline::Submit(Primitives::GetQuadMesh(), materials, Vector3::Zero, m_world_matrix_buffer_->GetAddress());
}
}

CEREAL_REGISTER_TYPE(engine::Image)