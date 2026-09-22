#include "pch.h"
#include "layer.h"

#include "gui.h"

namespace engine
{
void Layer::OnInspectorGui()
{
    if (!ImGui::CollapsingHeader("Rendering Layer"))
        return;
    
    if (ImGui::CollapsingHeader("Names"))
    {
        for (int i = 0; i < m_layer_names_.size(); ++i)
        {
            ImGui::PushID(i);
            Gui::PropertyField(("Layer " + std::to_string(i)).c_str(), m_layer_names_[i]);
            ImGui::PopID();
        }
    }

    for (int i = 0; i < kLayerCount; ++i)
    {
        ImGui::PushID(i);
        bool current = m_masks_ & 1 << i;
        if (Gui::PropertyField(m_layer_names_[i].c_str(), current))
            m_masks_ ^= 1 << i;
        
        ImGui::PopID();
    }

    if (ImGui::Button("Enable All"))
        m_masks_ = UINT16_MAX;
    if (ImGui::Button("Disable All"))
        m_masks_ = 0;
}

void Layer::SetLayer(const uint32_t layer, const bool enabled)
{
    m_masks_ &= enabled << layer;
}

bool Layer::IsLayerEnabled(const uint32_t layer) const
{
    return m_masks_ & (1 << layer);
}

bool Layer::IsAnyLayerEnabled(const Layer& other) const
{
    return (m_masks_ & other.m_masks_) != 0;
}

bool Layer::IsAllLayerEnabled(const Layer& other) const
{
    return m_masks_ == other.m_masks_;
}
}