#include "pch.h"

#include "component.h"

#include "game_object.h"

namespace engine
{
Component::Component() : Object()
{}
void Component::OnInspectorGui()
{
    ImGui::Text("OnInspectorGui not implemented for %s", typeid(*this).name());
}

template <class Archive>
void Component::serialize(Archive &ar)
{
    ar(cereal::base_class<Object>(this), m_game_object_);
}
}

template void engine::Component::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive &);
template void engine::Component::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive &);
template void engine::Component::serialize<cereal::XMLOutputArchive>(cereal::XMLOutputArchive &);
template void engine::Component::serialize<cereal::XMLInputArchive>(cereal::XMLInputArchive &);

CEREAL_REGISTER_TYPE(engine::Component)