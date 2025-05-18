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

template void engine::Component::serialize<cereal::JSONOutputArchive>( cereal::JSONOutputArchive & );
template void engine::Component::serialize<cereal::JSONInputArchive>( cereal::JSONInputArchive & );

CEREAL_REGISTER_TYPE(engine::Component)