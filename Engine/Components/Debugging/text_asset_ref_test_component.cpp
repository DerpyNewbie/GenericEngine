#include "pch.h"
#include "text_asset_ref_test_component.h"

#include "gui.h"

namespace engine
{
void TextAssetRefTestComponent::OnInspectorGui()
{
    // PropertyField can query any Object
    Gui::PropertyField<Object>("Any", m_any_);

    // PropertyField can filter any Object by type
    Gui::PropertyField<class GameObject>("Any GameObject", m_any_);

    // PropertyField can take GameObject's component by drag and drop
    Gui::PropertyField<Transform>("Any Transform", m_any_);

    // ExpandablePropertyField can show the inspector of an assigned object
    Gui::ExpandablePropertyField("Text Asset", m_text_asset_);

    // ExpandablePropertyField can recursively show the inspector of an assigned object
    Gui::ExpandablePropertyField<TextAssetRefTestComponent>("Other Text Asset Ref", m_other_text_asset_ref_);
}
}

CEREAL_REGISTER_TYPE(engine::TextAssetRefTestComponent)