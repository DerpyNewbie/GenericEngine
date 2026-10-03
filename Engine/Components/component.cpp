#include "pch.h"

#include "component.h"
#include "game_object.h"

namespace engine
{
void Component::EnsurePrepared()
{
    InvokeOnStart();
}

void Component::InvokeOnAwake()
{
    if (!m_has_called_awake_)
    {
        OnAwake();
        m_has_called_awake_ = true;
    }
}
void Component::InvokeOnStart()
{
    InvokeOnEnabled();

    if (!m_has_called_start_)
    {
        OnStart();
        m_has_called_start_ = true;
    }
}

void Component::InvokeOnEnabled()
{
    InvokeOnAwake();

    if (!m_has_called_enabled_)
    {
        OnEnabled();
        m_has_called_enabled_ = true;
        m_has_called_disabled_ = false;
    }
}

void Component::InvokeOnDisabled()
{
    if (!m_has_called_disabled_)
    {
        OnDisabled();
        m_has_called_enabled_ = false;
        m_has_called_disabled_ = true;
    }
}

void Component::InvokeOnUpdate()
{
    EnsurePrepared();
    OnUpdate();
}

void Component::InvokeOnFixedUpdate()
{
    EnsurePrepared();
    OnFixedUpdate();
}


Component::Component() : Object()
{ }
void Component::OnInspectorGui()
{
    ImGui::Text("OnInspectorGui not implemented for %s", typeid(*this).name());
}

std::shared_ptr<GameObject> Component::GameObject() const
{
    return m_game_object_.lock();
}

template <class Archive>
void Component::serialize(Archive &ar, const uint32_t version)
{
    ar(cereal::base_class<Object>(this), m_game_object_);
}
}

template void engine::Component::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive &, uint32_t);
template void engine::Component::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive &, uint32_t);
template void engine::Component::serialize<cereal::XMLOutputArchive>(cereal::XMLOutputArchive &, uint32_t);
template void engine::Component::serialize<cereal::XMLInputArchive>(cereal::XMLInputArchive &, uint32_t);
template void engine::Component::serialize<cereal::PortableBinaryOutputArchive>(cereal::PortableBinaryOutputArchive &, uint32_t);
template void engine::Component::serialize<cereal::PortableBinaryInputArchive>(cereal::PortableBinaryInputArchive &, uint32_t);

CEREAL_REGISTER_TYPE(engine::Component)