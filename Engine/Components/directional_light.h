#pragma once
#include "component.h"
#include "light.h"
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"

namespace engine
{
struct DirectionalLightProperty : LightProperty
{
    Vector3 direction;
};

class DirectionalLight : public Light
{
    static std::vector<std::shared_ptr<DirectionalLight>> m_directional_lights_;
    static std::shared_ptr<StructuredBuffer> m_directional_lights_buffer_;
    static int m_last_directional_light_count_;

    DirectionalLightProperty m_property_ = {};

public:
    void OnAwake() override;
    void OnUpdate() override;

    static void SetDescHandle();
    static int LightCount();
    static void UpdateBuffer();

    float GetIntencity() override;
    void SetIntensity(float intensity) override;
    Color GetColor() override;
    void SetColor(Color color) override;
};
}