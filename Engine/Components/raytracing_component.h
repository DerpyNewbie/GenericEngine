#pragma once
#include "camera_component.h"
#include "component.h"
#include "Asset/asset_ptr.h"
#include "Rendering/uav_texture.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"
#include "Rendering/raytracing/raytracing_shader.h"
#include "Rendering/raytracing/shader_table.h"

namespace engine
{
class RaytracingComponent : public Component, public IRenderReceiver
{
    AssetPtr<UavTexture> m_output_texture_;

    AssetPtr<CameraComponent> m_target_camera_;

    std::shared_ptr<ShaderTable> m_shader_table_;
    AssetPtr<RaytracingShader> m_raytracing_shader_;

public:
    void OnInspectorGui() override;

    void OnEnabled() override;
    void OnDisabled() override;

    void Render() override;

    std::shared_ptr<Texture2D> GetResult();
    AssetPtr<CameraComponent> TargetCamera();

    void SetTexture(const AssetPtr<UavTexture> &uav_texture);
    void SetTargetCamera(const AssetPtr<CameraComponent> &target_camera);
    void SetRaytracingShader(const AssetPtr<RaytracingShader> &raytracing_shader);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_target_camera_),
            CEREAL_NVP(m_output_texture_),
            CEREAL_NVP(m_raytracing_shader_)

        );
    }
};
}

CEREAL_CLASS_VERSION(engine::RaytracingComponent, 1)