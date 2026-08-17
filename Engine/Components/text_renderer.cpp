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
    Gui::PropertyField("Render Queue", render_queue);
    Gui::PropertyField("position", position);
    Gui::PropertyField("Rotation", rotation);
    Gui::PropertyField("Origin", origin);
    Gui::PropertyField("Scale", scale);

    Gui::PropertyField<FontData>("font data", font_data);
    char buf[256];
    strncpy_s(buf, sizeof(buf), string.c_str(), _TRUNCATE);
    buf[sizeof(buf) - 1] = '\0';

    if (ImGui::InputText("text", buf, sizeof(buf), 0))
    {
        string = buf;
    }
    Gui::PropertyField("color", color);
}

void TextRenderer::Render()
{
    RenderPipeline::Submit(font_data, position, string, color, rotation, origin, scale, render_queue);
}

Matrix TextRenderer::BoundsOrigin()
{
    return RenderPipeline::GetCurrentCamera().GetWorldMatrix();
}
}

CEREAL_REGISTER_TYPE(engine::TextRenderer)