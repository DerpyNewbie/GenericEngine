#pragma once
#include "component.h"
#include "Asset/asset_ptr.h"
#include "Asset/text_asset.h"

namespace engine
{
class TextAssetRefTestComponent : public Component
{
    IAssetPtr m_any_;
    AssetPtr<TextAsset> m_text_asset_;
    IAssetPtr m_other_text_asset_ref_;

public:
    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), m_text_asset_);
    }
};
}