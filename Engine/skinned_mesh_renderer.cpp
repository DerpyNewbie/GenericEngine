#include "skinned_mesh_renderer.h"

void engine::SkinnedMeshRenderer::OnDraw()
{
    MV1DrawModel(m_model_handle_);
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)

CEREAL_REGISTER_POLYMORPHIC_RELATION(engine::Component, engine::SkinnedMeshRenderer)