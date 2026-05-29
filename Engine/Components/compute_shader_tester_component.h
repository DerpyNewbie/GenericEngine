#pragma once
#include "compute_shader_component.h"
#include "Asset/asset_ptr.h"

namespace engine
{
struct ParticleData
{
    Vector2 pos;
    Vector2 velocity;
};

class ComputeShaderTesterComponent : public Component
{
    std::shared_ptr<StructuredBuffer> m_structured_buffer_;
    AssetPtr<ComputeShaderComponent> m_compute_shader_;

    void CreateBuffer();

public:
    void OnInspectorGui() override;
    void OnUpdate() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_compute_shader_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ComputeShaderTesterComponent, 1)