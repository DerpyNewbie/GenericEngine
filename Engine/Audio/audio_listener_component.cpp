#include "pch.h"

#include "audio_listener_component.h"
#include "audio.h"

namespace engine
{
std::list<std::weak_ptr<AudioListenerComponent>> AudioListenerComponent::m_listeners_;

void AudioListenerComponent::OnEnabled()
{
    m_listeners_.emplace_back(shared_from_base<AudioListenerComponent>());
}
void AudioListenerComponent::OnDisabled()
{
    erase_if(m_listeners_, [this](const std::weak_ptr<AudioListenerComponent> &listener) {
        return listener.lock()->Equals(this);
    });
}

void AudioListenerComponent::OnInspectorGui()
{
    ImGui::Text(ActiveListener()->Equals(this)
                    ? "Active Audio Listener"
                    : "Inactive Audio Listener");
}

std::shared_ptr<AudioListenerComponent> AudioListenerComponent::ActiveListener()
{
    if (m_listeners_.empty())
        return nullptr;

    return m_listeners_.front().lock();
}
}

CEREAL_REGISTER_TYPE(engine::AudioListenerComponent)