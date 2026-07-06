#pragma once
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class RenderTexture : public Texture2D
{
    friend class RenderTextureBuffer;

    bool m_allow_uav_;
    
public:
    RenderTexture();
    
    void OnInspectorGui() override;

    bool AllowUav() const;
    
    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Texture2D>(this),
            CEREAL_NVP(m_allow_uav_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::RenderTexture, 1)