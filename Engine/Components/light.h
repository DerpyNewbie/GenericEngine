#pragma once
#include "component.h"
#include "Rendering/light_data.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
enum class kLightType : uint8_t
{
    kDirectional,
    kSpotLight,
};

class Light : public Component
{
    friend class CameraComponent;
    friend class RenderPipeline;
    friend class Lighting;

protected:
    std::vector<int> m_depth_texture_handle_;
    LightData m_light_data_;
    bool m_has_shadow_;

public:
    void OnInspectorGui() override;
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    virtual void UpdateData() = 0;
    virtual bool InCameraView(const std::array<Vector3, 8> &frustum) = 0;
    virtual Vector3 GetPos() = 0;
    virtual int ShadowMapCount() = 0;
    virtual std::vector<Matrix> CalcViewProj(const std::array<Vector3, 8> &frustum_corners) = 0;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_light_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Light, 1)