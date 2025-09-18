#pragma once
#include "component.h"
#include "Rendering/camera.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
struct alignas(16) LightData
{
    int type = 1;
    int cast_shadow = 1;
    float intensity = 1;
    float range = 1;

    DirectX::XMFLOAT4 pos;
    DirectX::XMFLOAT4 direction;
    Color color;

    float inner_cos;
    float outer_cos;
    float padding[2]; // ★ HLSL の float2 padding と対応させる

    DirectX::XMFLOAT4X4 view_proj;
};
static_assert(sizeof(LightData) == 144, "LightData size mismatch");


struct alignas(256) LightCountBuffer
{
    uint32_t count;
};

class Light : public Component
{
    friend Camera;
    friend RenderPipeline;

    constexpr static int kMaxLightCount = 10;
    static int m_last_light_count_;
    static std::shared_ptr<StructuredBuffer> m_lights_buffer_;
    static std::shared_ptr<ConstantBuffer> m_light_count_buffer_;
    static std::shared_ptr<DescriptorHandle> m_lights_buffer_handle_;

    static void UpdateLightCountBuffer();
    static void UpdateLightBuffer();
    static void SetLightCountBuffer();
    static void SetLightBuffer();
    static void SetBuffers();

    void SetMatrix();

protected:
    static std::vector<std::shared_ptr<Light>> m_lights_;
    LightData m_light_data_;
    std::shared_ptr<Camera> m_camera_;

public:
    void OnInspectorGui() override;
    void OnEnabled() override;
    void OnUpdate() override;
};
}