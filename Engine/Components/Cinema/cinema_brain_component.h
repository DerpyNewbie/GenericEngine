#pragma once
#include "cinema_camera_component.h"

namespace engine
{
class CinemaBrainComponent : public Component
{
    AssetPtr<Camera> m_target_camera_;

    // should be a pair of <AssetPtr<CinemaCameraComponent>, float>
    std::vector<std::pair<AssetPtr<CinemaCameraComponent>, float>> m_cinema_cameras_;

public:
    void OnInspectorGui() override;
    void OnUpdate() override;

    void AddCamera(const std::shared_ptr<CinemaCameraComponent> &cinema_camera, float blend_coefficient);
    void RemoveCamera(std::shared_ptr<CinemaCameraComponent> cinema_camera);


    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_target_camera_), CEREAL_NVP(m_cinema_cameras_));
    }
};
}