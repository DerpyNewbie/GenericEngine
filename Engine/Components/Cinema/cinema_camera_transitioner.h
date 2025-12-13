#pragma once
#include "Components/component.h"
#include "Asset/asset_ptr.h"
#include "cinema_brain_component.h"
#include "cinema_camera_component.h"

namespace engine
{
class CinemaCameraTransitioner : public Component
{
    AssetPtr<CinemaCameraComponent> m_from_camera_;
    AssetPtr<CinemaCameraComponent> m_to_camera_;
    AssetPtr<CinemaBrainComponent> m_cinema_brain_;

public:
    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_from_camera_),
            CEREAL_NVP(m_to_camera_),
            CEREAL_NVP(m_cinema_brain_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::CinemaCameraTransitioner, 1)