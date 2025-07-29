#include "pch.h"
#include "audio.h"

#include "str_util.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

std::unique_ptr<AudioEngine> Audio::m_AudioEngine_;

void Audio::Initialize()
{
    if (m_AudioEngine_)
        return;
    AUDIO_ENGINE_FLAGS flags = AudioEngine_Default;
#ifdef _DEBUG
    flags |= AudioEngine_Debug;
#endif

    m_AudioEngine_ = std::make_unique<AudioEngine>(flags);
}

void Audio::LoadWav(const std::wstring &file_path)
{
    m_SoundEffect = std::make_unique<SoundEffect>(m_AudioEngine_.get(), file_path.c_str());
    m_SoundInstance = m_SoundEffect->CreateInstance();
}

void Audio::LoadWav(const std::string &file_path)
{
    LoadWav(StringUtil::ConvertToWString(file_path));
}

void Audio::Apply3D(Matrix listener_mat, Matrix emitter_mat)
{
    AudioListener listener;
    listener.SetPosition(listener_mat.Translation());
    listener.SetOrientation(listener_mat.Forward(), Vector3(0, 1, 0));

    AudioEmitter emitter;
    emitter.SetPosition(emitter_mat.Translation());
    emitter.SetOrientation(emitter_mat.Forward(), Vector3(0, 1, 0));
    m_SoundInstance->Apply3D(listener, emitter, true);
}

void Audio::Play(bool is_loop)
{
    m_SoundInstance->Play(is_loop);
}

void Audio::SetVolume(float volume)
{
    m_SoundInstance->SetVolume(volume);
}

SoundState Audio::SoundState()
{
    return m_SoundInstance->GetState();
}