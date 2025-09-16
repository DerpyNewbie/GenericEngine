#pragma once
#include "component.h"
#include "Rendering/camera.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
struct LightData
{
    int type;
    int cast_shadow = 1; //GPUに送るのでINTで扱う
    Vector3 pos;
    Vector3 direction;
    Color color;
    float intensity = 1;
    float range;
    float inner_cos;
    float outer_cos;
    DirectX::XMMATRIX view_proj;
};

class Light : public Component
{
    friend Camera;
    friend RenderPipeline;
    constexpr static int kMaxLightCount = 10;
    static int m_last_light_count_;
    static std::shared_ptr<StructuredBuffer> m_lights_buffer_;
    static std::shared_ptr<ConstantBuffer> m_light_count_buffer_;

    static void UpdateLightCountBuffer();
    static void UpdateLightBuffer();
    static void SetLightCountBuffer();
    static void SetLightBuffer();
    static void SetBuffers();

protected:
    static std::vector<std::shared_ptr<Light>> m_lights_;
    LightData m_light_data_;
    std::shared_ptr<Camera> m_camera_;

public:
    void OnInspectorGui() override;
    void OnUpdate() override;
};
}