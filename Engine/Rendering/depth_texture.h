#pragma once
#include "CabotEngine/Graphics/Texture2D.h"
#include "CabotEngine/Graphics/Texture2DArray.h"

namespace engine
{
class DepthTexture : public Texture2D
{
public:
    
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