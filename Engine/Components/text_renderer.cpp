#include "pch.h"
#include "gui.h"
#include "text_renderer.h"

#include "rect_transform.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

using namespace DirectX;

namespace engine
{
void TextRenderer::OnInspectorGui()
{
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

void TextRenderer::Render()
{
    if (!font_data.Lock())
    {
        return;
    }

    const auto position = GameObject()->GetComponent<RectTransform>()->CalculateScreenRect().pos;

    const auto sprite_batch = FontData::SpriteBatch();
    const auto sprite_font = font_data.CastedLock()->SpriteFont();
    sprite_batch->Begin(RenderEngine::CommandList());
    sprite_font->DrawString(sprite_batch.get(), string.c_str(), position, color, rotation, origin, scale);
    sprite_batch->End();
    RenderEngine::CommandList()->SetGraphicsRootSignature(RootSignature::Get());
}
}

CEREAL_REGISTER_TYPE(engine::TextRenderer)