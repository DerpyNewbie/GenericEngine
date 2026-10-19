#pragma once
#include "CabotEngine/Graphics/Texture2D.h"
#include "CabotEngine/Graphics/Texture2DArray.h"

namespace engine
{
/// <summary>
/// 深度値を書き込むためのTextureです。
/// </summary>
class DepthTexture : public Texture2D
{
public:
    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Texture2D>(this)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::DepthTexture, 1)