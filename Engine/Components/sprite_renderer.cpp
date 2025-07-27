#include <DirectXColors.h>
#include "sprite_renderer.h"
#include "gui.h"
#include "Rendering/CabotEngine/Engine/InputManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

#include <Rendering/CabotEngine/Graphics/RootSignatureManager.h>
using namespace DirectX;

void engine::SpriteRenderer::OnInspectorGui()
{
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

void engine::SpriteRenderer::OnDraw()
{
    if (!font_data.Lock())
    {
        return;
    }
    auto sprite_batch = FontData::SpriteBatch();
    auto sprite_font = font_data.CastedLock()->SpriteFont();
    sprite_batch.lock()->Begin(g_RenderEngine->CommandList());
    sprite_font->DrawString(sprite_batch.lock().get(), string.c_str(), position, color);
    sprite_batch.lock()->End();
    g_RenderEngine->CommandList()->SetGraphicsRootSignature(g_RootSignatureManager.Get("Basic"));
}

std::weak_ptr<engine::Transform> engine::SpriteRenderer::GetTransform()
{
    return GameObject()->Transform();
}