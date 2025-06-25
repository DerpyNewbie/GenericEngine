#include "pch.h"
#include "text_asset.h"

void engine::TextAsset::OnInspectorGui()
{
    ImGui::InputTextMultiline("Content", &content);
}