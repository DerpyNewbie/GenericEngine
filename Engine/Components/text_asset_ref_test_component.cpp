#include "pch.h"
#include "text_asset_ref_test_component.h"

#include "Editor/gui.h"
void engine::TextAssetRefTestComponent::OnInspectorGui()
{
    editor::Gui::PropertyField<TextAsset>("Text Asset", m_text_asset_, ".txt");
    if (m_text_asset_.IsLoaded())
        std::dynamic_pointer_cast<TextAsset>(m_text_asset_.Lock())->OnInspectorGui();
    else
        ImGui::Text("Not loaded");
}