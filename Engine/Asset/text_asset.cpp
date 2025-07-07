#include "pch.h"
#include "text_asset.h"

void engine::TextAsset::OnInspectorGui()
{
    ImGui::InputTextMultiline("Content", &content);
    ImGui::Separator();
    for (auto &[key, value] : key_value_pairs)
    {
        ImGui::InputText(key.c_str(), &value);
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Remove"))
            {
                key_value_pairs.erase(key);
                ImGui::EndPopup();
                break;
            }
            ImGui::EndPopup();
        }
    }
    ImGui::Separator();
    static std::string buff;
    ImGui::InputText("###TEXT_ASSET_ADD_KEY", &buff);
    ImGui::SameLine();
    if (ImGui::Button("Add Key"))
    {
        key_value_pairs[buff] = "";
        buff = "";
    }
}