#pragma once
#include "component.h"
#include "Asset/asset_ptr.h"
#include "Cinema/cinema_brain_component.h"
#include "Cinema/cinema_camera_component.h"

namespace engine
{
class CameraTransitioner : public Component
{
    AssetPtr<CinemaCameraComponent> m_from_camera_;
    AssetPtr<CinemaCameraComponent> m_to_camera_;
    AssetPtr<CinemaBrainComponent> m_cinema_brain_;

public:
    void OnInspectorGui() override;
};
}