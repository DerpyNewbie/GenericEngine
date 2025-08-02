#pragma once
#include "Components/component.h"

namespace engine
{
class AudioListenerComponent : public Component
{
    friend class Audio;
    static std::list<std::weak_ptr<AudioListenerComponent>> m_listeners_;

public:
    void OnEnabled() override;
    void OnDisabled() override;
    void OnInspectorGui() override;

    static std::shared_ptr<AudioListenerComponent> ActiveListener();
};
}