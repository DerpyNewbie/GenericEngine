#pragma once
#include "audio_clip.h"
#include "Asset/asset_ptr.h"
#include "Components/component.h"

namespace engine
{
/// <summary>
/// AudioClipを再生するComponentです。
/// </summary>
class AudioSourceComponent : public Component
{
    friend class Audio;

    std::shared_ptr<DirectX::SoundEffectInstance> m_sound_effect_instance_;
    DirectX::AudioEmitter m_emitter_;
    AssetPtr<AudioClip> m_clip_;
    float m_volume_ = 1.0f;
    float m_pitch_ = 0;
    float m_doppler_factor_ = 1.0f;
    bool m_loop_ = false;
    bool m_use_3d_ = true;
    bool m_play_on_start_ = false;

public:
    /// <summary>
    /// PlayOnStartが有効な場合に再生を開始します。
    /// </summary>
    void OnStart() override;
    /// <summary>
    /// 再生用のインスタンスがある場合、再生を再開します。
    /// </summary>
    void OnEnabled() override;
    /// <summary>
    /// Loopが有効で再生が止まっている場合は再生し直し、3Dが有効な場合は現在のListenerに対する3D音響を適用します。
    /// </summary>
    void OnUpdate() override;
    /// <summary>
    /// 再生用のインスタンスがある場合、再生を一時停止します。
    /// </summary>
    void OnDisabled() override;

    void OnInspectorGui() override;

    /// <summary>
    /// 再生するClipを設定します。再生中の場合は停止します。
    /// </summary>
    /// <param name="clip">再生するClip</param>
    void SetClip(const AssetPtr<AudioClip> &clip);
    /// <summary>
    /// 音量を設定します。負の値は絶対値として扱われます。
    /// </summary>
    /// <param name="volume">音量</param>
    void SetVolume(float volume);
    /// <summary>
    /// Pitchを設定します。-1 ～ 1 の範囲に制限されます。
    /// </summary>
    /// <param name="pitch">Pitch</param>
    void SetPitch(float pitch);
    /// <summary>
    /// Listenerの速度に掛ける、ドップラー効果の係数を設定します。
    /// </summary>
    /// <param name="factor">係数</param>
    void SetDopplerFactor(float factor);
    /// <summary>
    /// ループ再生するかどうかを設定します。
    /// </summary>
    /// <param name="loop">ループ再生する場合 true</param>
    void SetLoop(bool loop);
    /// <summary>
    /// 3D音響を使うかどうかを設定します。再生中の音には反映されないので、再生し直す必要があります。
    /// </summary>
    /// <param name="use_3d">3D音響を使う場合 true</param>
    void SetUse3D(bool use_3d);
    /// <summary>
    /// OnStartで自動的に再生するかどうかを設定します。
    /// </summary>
    /// <param name="play_on_start">自動的に再生する場合 true</param>
    void SetPlayOnStart(bool play_on_start);

    /// <summary>
    /// 設定されているClipを取得します。
    /// </summary>
    [[nodiscard]] AssetPtr<AudioClip> Clip() const;
    /// <summary>
    /// 音量を取得します。
    /// </summary>
    [[nodiscard]] float Volume() const;
    /// <summary>
    /// Pitchを取得します。
    /// </summary>
    [[nodiscard]] float Pitch() const;
    /// <summary>
    /// ループ再生が有効であるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsLooping() const;
    /// <summary>
    /// 3D音響が有効であるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool Use3D() const;
    /// <summary>
    /// OnStartで自動的に再生するかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool DoPlayOnStart() const;

    /// <summary>
    /// 再生中であるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsPlaying() const;
    /// <summary>
    /// 一時停止中であるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsPaused() const;
    /// <summary>
    /// 停止しているかどうかを取得します。一度も再生されていない場合も true を返します。
    /// </summary>
    [[nodiscard]] bool IsStopped() const;

    /// <summary>
    /// 設定されているClipを最初から再生します。
    /// </summary>
    void Play();
    /// <summary>
    /// 再生を一時停止します。
    /// </summary>
    void Pause() const;
    /// <summary>
    /// 一時停止した再生を再開します。
    /// </summary>
    void Resume() const;
    /// <summary>
    /// 再生を停止し、再生用のインスタンスを破棄します。
    /// </summary>
    void Stop();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_clip_),
            CEREAL_NVP(m_volume_),
            CEREAL_NVP(m_pitch_),
            CEREAL_NVP(m_doppler_factor_),
            CEREAL_NVP(m_loop_),
            CEREAL_NVP(m_use_3d_),
            CEREAL_NVP(m_play_on_start_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::AudioSourceComponent, 1)