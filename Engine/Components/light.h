#pragma once
#include "component.h"
#include "Rendering/camera.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"

namespace engine
{
struct LightProperty
{
    Color color;
    float m_intensity_;
};

enum kLightKind
{
    kDirectional,

    kLightKindCount
};

class Light : public Component
{
    friend Camera;
    friend RenderPipeline;
    static std::shared_ptr<ConstantBuffer> m_light_count_buffer_;

    static void CreateLightCountBuffer();
    static void UpdateLightCountBuffer();
    static void SetLightCountBuffer();
    static void SetLightBuffers();

protected:
    bool m_has_shadow_ = false;

public:
    void OnInspectorGui() override;
    virtual Color GetColor() = 0;
    virtual void SetColor(Color color) = 0;
};
}