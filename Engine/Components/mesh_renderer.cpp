#include "pch.h"

#include "mesh_renderer.h"
#include "game_object.h"
#include "gui.h"
#include "Rendering/gizmos.h"
#include "Rendering/crs_bridge.h"

namespace engine
{
bool MeshRenderer::m_draw_bounds_ = false;

void MeshRenderer::RecalculateBoundingBox()
{
    auto min_pos = Vector3(0, 0, 0);
    auto max_pos = Vector3(0, 0, 0);
    for (int i = 0; i < m_shared_mesh_->vertices.size(); ++i)
    {
        auto vertex = m_shared_mesh_->vertices[i];
        min_pos.x = std::min(min_pos.x, vertex.x);
        min_pos.y = std::min(min_pos.y, vertex.y);
        min_pos.z = std::min(min_pos.z, vertex.z);

        max_pos.x = std::max(max_pos.x, vertex.x);
        max_pos.y = std::max(max_pos.y, vertex.y);
        max_pos.z = std::max(max_pos.z, vertex.z);
    }

    DirectX::BoundingBox::CreateFromPoints(bounds, min_pos, max_pos);
}

void MeshRenderer::OnInspectorGui()
{
    ImGui::Checkbox("Draw Bounds", &m_draw_bounds_);
    Renderer::OnInspectorGui();

    Gui::ExpandablePropertyField("Mesh", m_shared_mesh_);

    if (ImGui::CollapsingHeader("Materials"))
    {
        ImGui::Indent();
        for (auto i = 0; i < shared_materials.size(); ++i)
        {
            if (ImGui::CollapsingHeader(("Material " + std::to_string(i)).c_str()))
            {
                ImGui::Indent();
                ImGui::PushID(i);
                Gui::ExpandablePropertyField("Material" + i, shared_materials[i]);
                ImGui::PopID();
                ImGui::Unindent();
            }
        }
        ImGui::Unindent();
    }
}

void MeshRenderer::Render()
{
    const auto &world_matrix = GameObject()->Transform()->WorldMatrix();
    SubmitToRenderer(world_matrix, bounds);
}

void MeshRenderer::SubmitToRenderer(const Matrix &world, const DirectX::BoundingBox &local_bounds,
                                    const CRS::BufferHandle bone_buffer)
{
    CrsBridge::SubmitMeshRenderer(m_crs_id_, m_shared_mesh_, shared_materials, world, local_bounds,
                                  bone_buffer);
}

void MeshRenderer::OnVisibilityChanged(const bool visible)
{
    if (!visible)
        CrsBridge::RemoveMeshRenderer(m_crs_id_);
}

void MeshRenderer::SetSharedMesh(const AssetPtr<Mesh> &mesh)
{
    m_shared_mesh_ = mesh;
    RecalculateBoundingBox();
}

void MeshRenderer::DrawBounds()
{
    const auto matrix = BoundsOrigin();
    Gizmos::DrawBounds(bounds, Gizmos::kDefaultColor, matrix);
}

Matrix MeshRenderer::BoundsOrigin()
{
    return GameObject()->Transform()->WorldMatrix();
}
}

CEREAL_REGISTER_TYPE(engine::MeshRenderer)
