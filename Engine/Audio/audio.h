#pragma once
#include "audio_clip.h"
#include "audio_source_component.h"
#include "event_receivers.h"

#include <directxtk12/Audio.h>

namespace engine
{
/// <summary>
/// AudioEngineを管理し、音の再生とListenerの更新を行うクラスです。
/// </summary>
class Audio : public IUpdateReceiver
{
    friend class Engine;
    static std::shared_ptr<Audio> m_instance_;

    std::unique_ptr<DirectX::AudioEngine> m_audio_engine_;
    DirectX::AudioListener m_listener_;
    float m_doppler_factor_ = 1.0f;

    /// <summary>
    /// Audioのインスタンスを生成し、UpdateManagerに登録します。
    /// </summary>
    static void Init();

public:
    /// <summary>
    /// AudioEngineとAudioListenerを生成します。
    /// </summary>
    Audio();

    /// <returns>Initが呼ばれる前は nullptr</returns>
    static std::shared_ptr<Audio> Instance();

    /// <summary>
    /// ActiveなAudioListenerComponentのTransformからListenerの位置、向き、速度を更新し、AudioEngineを更新します。
    /// </summary>
    void OnUpdate() override;

    /// <summary>
    /// AudioSourceのClipから再生用のインスタンスを作成し、再生します。Clipが設定されていない場合は何もしません。
    /// </summary>
    /// <param name="audio_source">再生するAudioSource</param>
    void Play(const std::shared_ptr<AudioSourceComponent> &audio_source);

    /// <summary>
    /// 全体の音量を取得します。
    /// </summary>
    [[nodiscard]] float MasterVolume() const;
    /// <summary>
    /// Listenerの速度に掛ける、ドップラー効果の係数を取得します。
    /// </summary>
    [[nodiscard]] float DopplerFactor() const;
    /// <summary>
    /// AudioEngineの統計情報を取得します。
    /// </summary>
    [[nodiscard]] DirectX::AudioStatistics Statistics() const;
    /// <summary>
    /// 現在のListenerの状態を取得します。
    /// </summary>
    [[nodiscard]] DirectX::AudioListener AudioListener() const;

    /// <summary>
    /// 全体の音量を設定します。
    /// </summary>
    /// <param name="volume">音量</param>
    void SetMasterVolume(float volume) const;
    /// <summary>
    /// Listenerの速度に掛ける、ドップラー効果の係数を設定します。
    /// </summary>
    /// <param name="factor">係数</param>
    void SetDopplerFactor(float factor);

    /// <summary>
    /// 音声ファイルを読み込み、指定されたGuidでAudioClipを生成します。
    /// </summary>
    /// <param name="guid">生成するAudioClipのGuid</param>
    /// <param name="path">音声ファイルのPath</param>
    /// <returns>AudioEngineが初期化されていない場合 nullptr</returns>
    [[nodiscard]] std::shared_ptr<AudioClip> LoadAudioClip(xg::Guid guid, const std::filesystem::path &path) const;
};
}