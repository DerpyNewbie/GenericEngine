#pragma once
#include "component.h"
#include "Asset/asset_ptr.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class GlobalResourceTesterComponent : public Component
{
    AssetPtr<Texture2D> m_texture_;

public:
    void OnInspectorGui() override;
    void OnDeserialized() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_texture_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::GlobalResourceTesterComponent, 1)