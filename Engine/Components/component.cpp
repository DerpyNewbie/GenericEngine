#include "component.h"
#include "game_object.h"

namespace engine
{
Component::Component() : Object()
{}

template <class Archive>
void Component::serialize(Archive &ar)
{
    ar(cereal::base_class<Object>(this), m_game_object_);
}
}

template void engine::Component::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive &);
template void engine::Component::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive &);
// template void engine::Component::serialize<cereal::BinaryOutputArchive>(cereal::BinaryOutputArchive &);
// template void engine::Component::serialize<cereal::BinaryInputArchive>(cereal::BinaryInputArchive &);
template void engine::Component::serialize<cereal::XMLOutputArchive>(cereal::XMLOutputArchive &);
template void engine::Component::serialize<cereal::XMLInputArchive>(cereal::XMLInputArchive &);
template void engine::Component::serialize<cereal::PortableBinaryOutputArchive>(cereal::PortableBinaryOutputArchive &);
template void engine::Component::serialize<cereal::PortableBinaryInputArchive>(cereal::PortableBinaryInputArchive &);

CEREAL_REGISTER_TYPE(engine::Component)