#include "pch.h"
#include "renderer_2d.h"
#include "image.h"

#include "gui.h"
#include "rect_transform.h"
#include "Rendering/crs_bridge.h"

using namespace DirectX::SimpleMath;

namespace engine
{
Matrix Image::CalcScreenMatrix() const
{
    const auto rect = NormalizedRect();
    const auto scale_mat = Matrix::CreateScale(rect.size.x, rect.size.y, 1.0f);
    const auto trans_mat = Matrix::CreateTranslation(rect.pos.x, rect.pos.y, 0.0f);
    return scale_mat * trans_mat;
}

void Image::OnInspectorGui()
{
    Gui::ExpandablePropertyField("Material", shared_material);
}

void Image::Render()
{
    CrsBridge::AddUiImage(CalcScreenMatrix(), shared_material.CastedLock());
}
}

CEREAL_REGISTER_TYPE(engine::Image)
