#pragma once
#include "shader.h"
#include "Asset/asset_ptr.h"

namespace engine
{
struct RenderPass : Object, Inspectable
{
    std::vector<AssetPtr<Shader>> shaders;

    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(shaders)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::RenderPass, 1)