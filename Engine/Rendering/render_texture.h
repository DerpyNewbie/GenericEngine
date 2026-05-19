#pragma once
#include "CabotEngine/Graphics/ComPtr.h"
#include "CabotEngine/Graphics/Texture2D.h"

#include <directx/d3d12.h>

namespace engine
{
class RenderTexture : public Texture2D
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

CEREAL_CLASS_VERSION(engine::RenderTexture, 1)