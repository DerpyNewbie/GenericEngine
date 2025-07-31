#pragma once
#include "Rendering/CabotEngine/Engine/audio.h"

namespace engine
{
class AudioAsset : public Object
{
public:
    Audio audio;

    void OnConstructed() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this));
    }
};
}