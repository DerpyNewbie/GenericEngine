#pragma once
#include <directxtk12/Audio.h>
#include <directxtk12/SimpleMath.h>

namespace engine
{
class AudioClip : public Object
{
    friend class AudioImporter;

    std::unique_ptr<DirectX::SoundEffect> m_sound_effect_;
};
}