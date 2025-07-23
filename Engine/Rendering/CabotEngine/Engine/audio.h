#pragma once
#include <directxtk12/Audio.h>
#include <directxtk12/SimpleMath.h>

class Audio
{
    static std::unique_ptr<DirectX::AudioEngine> m_AudioEngine_;
    std::unique_ptr<DirectX::SoundEffect> m_SoundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance> m_SoundInstance;

public:
    void Initialize();
    void LoadWav(const std::wstring &file_path);
    void LoadWav(const std::string &file_path);
    void Apply3D(DirectX::SimpleMath::Matrix listener_mat, DirectX::SimpleMath::Matrix emitter_mat);
    void Play(bool is_loop);

    void SetVolume(float volume);
    DirectX::SoundState SoundState();
};