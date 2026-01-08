#include "pch.h"
#include "gui.h"
#include "text_renderer.h"
#include "Components/camera_component.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

using namespace DirectX;

namespace engine
{
void TextRenderer::OnInspectorGui()
{
    Gui::PropertyField("Position", position);
    Gui::PropertyField("Rotation", rotation);
    Gui::PropertyField("Origin", origin);
    Gui::PropertyField("Scale", scale);

    Gui::PropertyField("font data", font_data);
    char buf[256];
    strncpy_s(buf, sizeof(buf), string.c_str(), _TRUNCATE);
    buf[sizeof(buf) - 1] = '\0';

    if (ImGui::InputText("text", buf, sizeof(buf), 0))
    {
        string = buf;
    }
    Gui::PropertyField("Color", color);
}

void TextRenderer::OnEnabled()
{
    m_text_renderers_.emplace(shared_from_base<TextRenderer>());
}
void TextRenderer::OnDisabled()
{
    m_text_renderers_.erase(shared_from_base<TextRenderer>());
}

void TextRenderer::Render()
{
    if (!font_data.Lock())
    {
        return;
    }
    auto sprite_batch = FontData::SpriteBatch();
    auto sprite_font = font_data.CastedLock()->SpriteFont();
    sprite_batch->Begin(RenderEngine::CommandList());
    sprite_font->DrawString(sprite_batch.get(), string.c_str(), position, color, rotation, origin, scale);
    sprite_batch->End();
    RenderEngine::CommandList()->SetGraphicsRootSignature(RootSignature::Get());
}
}

CEREAL_REGISTER_TYPE(engine::TextRenderer)