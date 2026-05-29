#pragma once

#include "renderer.h"

namespace engine
{
class ProceduralRenderer : public Renderer
{
    uint32_t m_vertex_count_ = 0;

public:
    void OnConstructed() override;
    void OnInspectorGui() override;
    Matrix BoundsOrigin() override;

    void Render() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Renderer>(this),
            CEREAL_NVP(m_vertex_count_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ProceduralRenderer, 1)