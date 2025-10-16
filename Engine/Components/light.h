#pragma once
#include "component.h"
#include "Components/camera_component.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
struct alignas(16) LightData
{
    int type = 1;
    int cast_shadow = 0;
    float intensity = 1;
    float range = 1;

    Vector4 pos;
    Quaternion direction;
    Color color;

    float inner_cos;
    float outer_cos;
    float padding[2];

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(type),
           CEREAL_NVP(cast_shadow),
           CEREAL_NVP(intensity),
           CEREAL_NVP(range),
           CEREAL_NVP(pos),
           CEREAL_NVP(direction),
           CEREAL_NVP(color),
           CEREAL_NVP(inner_cos),
           CEREAL_NVP(outer_cos));
    }
};


struct alignas(256) LightCountBuffer
{
    ~LightCountBuffer() = default;
    uint32_t count;
};

class Light : public Component
{
    friend CameraComponent;
    friend RenderPipeline;

    static std::shared_ptr<StructuredBuffer> m_lights_buffer_;
    static std::shared_ptr<ConstantBuffer> m_light_count_buffer_;
    static std::shared_ptr<DescriptorHandle> m_lights_buffer_handle_;

    static void UpdateLightCountBuffer();
    static void UpdateLightBuffer();
    static void SetLightCountBuffer();
    static void SetLightBuffer();
    static void SetBuffers();

protected:
    std::vector<int> m_depth_texture_handle_;
    LightData m_light_data_;
    bool m_has_shadow_;

public:
    void OnInspectorGui() override;
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    virtual bool InCameraView(const std::array<Vector3, 8> &frustum) = 0;
    virtual Vector3 GetPos() = 0;
    virtual int ShadowMapCount() = 0;
    virtual std::vector<Matrix> CalcViewProj(const std::array<Vector3, 8> &frustum_corners) = 0;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_light_data_));
    }
};
}