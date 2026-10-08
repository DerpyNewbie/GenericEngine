#pragma once
#include "Components/component.h"

namespace engine
{
/// <summary>
/// 音を聞く位置を表すComponentです。
/// </summary>
class AudioListenerComponent : public Component
{
    friend class Audio;
    static std::list<std::weak_ptr<AudioListenerComponent>> m_listeners_;

public:
    void OnEnabled() override;
    void OnDisabled() override;
    void OnInspectorGui() override;

    /// <summary>
    /// 現在使用されているListener(リストの先頭のListener)を取得します。
    /// </summary>
    /// <returns>Listenerが1つもない場合 nullptr</returns>
    static std::shared_ptr<AudioListenerComponent> ActiveListener();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Component>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::AudioListenerComponent, 1)