#pragma once
#include <directxtk12/Audio.h>

namespace engine
{
/// <summary>
/// 音声データを持つAssetです。
/// </summary>
class AudioClip : public Object, public Inspectable
{
    friend class Audio;

    std::unique_ptr<DirectX::SoundEffect> m_sound_effect_;

public:
    void OnInspectorGui() override;

    /// <summary>
    /// 音声の長さを取得します。
    /// </summary>
    std::chrono::milliseconds Duration() const;
    /// <summary>
    /// チャンネル数を取得します。
    /// </summary>
    unsigned short Channels() const;
    /// <summary>
    /// SampleRate(Hz)を取得します。
    /// </summary>
    unsigned long SampleRate() const;
};
}