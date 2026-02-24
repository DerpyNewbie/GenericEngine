#include "pch.h"
#include "gui.h"
#include "text_renderer.h"
#include "Components/camera_component.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

using namespace DirectX;

namespace engine
{
void TextRenderer::OnInspectorGui()
{
    Renderer::OnInspectorGui();
    float pos[2];
    EngineUtil::ToFloat2(pos, position);
    if (ImGui::InputFloat2("position", pos))
    {
        position.x = pos[0];
        position.y = pos[1];
    }
    float col[4];
    EngineUtil::ToFloat4(col, color);
    if (ImGui::InputFloat4("color", col))
    {
        color.x = col[0];
        color.y = col[1];
        color.z = col[2];
        color.w = col[3];
    }

    Gui::PropertyField<FontData>("font data", font_data);
    char buf[256];
    strncpy_s(buf, sizeof(buf), string.c_str(), _TRUNCATE);
    buf[sizeof(buf) - 1] = '\0';

    if (ImGui::InputText("text", buf, sizeof(buf), 0))
    {
        string = buf;
    }
}

void TextRenderer::Render()
{
    RenderPipeline::Submit(font_data, position, string, color);
}

Matrix TextRenderer::BoundsOrigin()
{
    return RenderPipeline::GetCurrentCamera().GetWorldMatrix();
}
}

CEREAL_REGISTER_TYPE(engine::TextRenderer)