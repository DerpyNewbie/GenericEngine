#include "pch.h"
#include "audio_source.h"
#include "audio.h"
#include "gui.h"

using namespace DirectX;

void engine::AudioSource::OnInspectorGui()
{
    if (Gui::PropertyField("AudioClip", audio_clip))
    {
        if (audio_clip.Lock())
        {
            Audio::Subscribe(shared_from_base<AudioSource>());
            return;
        }
        Audio::UnSubscribe(shared_from_base<AudioSource>());

    }
}

void engine::AudioSource::Play() const
{
    m_sound_instance_->Play(is_loop_);
}

void engine::AudioSource::SetListenerMatrix(const Matrix &listener_matrix)
{
    if (!use_3d)
        return;
    AudioListener listener;
    listener.SetPosition(listener_matrix.Translation());
    listener.SetOrientation(listener_matrix.Forward(), Vector3(0, 1, 0));

    auto emitter_matrix = GameObject()->Transform()->WorldMatrix();
    AudioEmitter emitter;
    emitter.SetPosition(emitter_matrix.Translation());
    emitter.SetOrientation(emitter_matrix.Forward(), Vector3(0, 1, 0));
    m_sound_instance_->Apply3D(listener, emitter);
}

bool engine::AudioSource::IsPlaying() const
{
    return m_sound_instance_->GetState() == PLAYING;
}