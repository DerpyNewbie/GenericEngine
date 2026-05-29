#include "pch.h"
#include "procedural_renderer.h"
#include "Rendering/render_pipeline.h"

namespace engine
{
void ProceduralRenderer::OnConstructed()
{
    shared_materials.emplace_back(AssetPtr<Material>::FromInstance(Instantiate<Material>()));
}

void ProceduralRenderer::OnInspectorGui()
{
    int vertex_count = static_cast<int>(m_vertex_count_);
    if (Gui::PropertyField("Vertex Count", vertex_count))
    {
        m_vertex_count_ = vertex_count;
    }

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

Matrix ProceduralRenderer::BoundsOrigin()
{
    return RenderPipeline::GetCurrentCamera().GetWorldMatrix();
}

void ProceduralRenderer::Render()
{
    RenderPipeline::Submit(shared_materials, m_vertex_count_);
}
}

CEREAL_REGISTER_TYPE(engine::ProceduralRenderer)