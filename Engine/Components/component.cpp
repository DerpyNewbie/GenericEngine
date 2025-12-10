#include "pch.h"

#include "component.h"
#include "game_object.h"

namespace engine
{
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