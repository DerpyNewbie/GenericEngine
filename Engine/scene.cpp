#include "scene.h"

#include "game_object.h"
#include "update_manager.h"

#include "cereal/archives/json.hpp"

namespace engine
{
void Scene::OnFixedUpdate()
{
    for (const auto &game_object : m_root_game_objects_)
    {
        game_object->InvokeFixedUpdate();
    }
}

void Scene::OnConstructed()
{
    UpdateManager::SubscribeUpdate(shared_from_base<Scene>());
    UpdateManager::SubscribeFixedUpdate(shared_from_base<Scene>());
}

void Scene::OnUpdate()
{
    auto buff = m_root_game_objects_;
    for (const auto &game_object : buff)
    {
        game_object->InvokeUpdate();
    }
}

const std::vector<std::shared_ptr<GameObject>> &Scene::RootGameObjects()
{
    return m_root_game_objects_;
}

template <class Archive>
void Scene::serialize(Archive &ar)
{
    ar(cereal::base_class<Object>(this), CEREAL_NVP(m_root_game_objects_), CEREAL_NVP(m_all_game_objects_));
}
}

template void engine::Scene::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive &);
template void engine::Scene::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive &);


CEREAL_REGISTER_TYPE(engine::Scene)