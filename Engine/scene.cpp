#include "pch.h"

#include "scene.h"

#include "game_object.h"
#include "update_manager.h"

namespace engine
{
void Scene::OnConstructed()
{
    UpdateManager::SubscribeUpdate(shared_from_base<Scene>());
    UpdateManager::SubscribeFixedUpdate(shared_from_base<Scene>());
}
void Scene::OnUpdate()
{
    bool has_destroying_game_object = false;
    for (const auto &game_object : m_root_game_objects_)
    {
        if (game_object->IsDestroying())
        {
            has_destroying_game_object = true;
            continue;
        }

        game_object->InvokeUpdate();
    }

    if (has_destroying_game_object)
    {
        std::erase_if(m_root_game_objects_, [](const auto &go) {
            return go->IsDestroying();
        });
    }
}
void Scene::OnFixedUpdate()
{
    for (const auto &game_object : m_root_game_objects_)
    {
        game_object->InvokeFixedUpdate();
    }
}
void Scene::OnDestroy()
{
    UpdateManager::UnsubscribeUpdate(shared_from_base<Scene>());
    UpdateManager::UnsubscribeFixedUpdate(shared_from_base<Scene>());
    for (const auto &game_object : m_root_game_objects_)
    {
        DestroyImmediate(game_object);
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
template void engine::Scene::serialize<cereal::XMLOutputArchive>(cereal::XMLOutputArchive &);
template void engine::Scene::serialize<cereal::XMLInputArchive>(cereal::XMLInputArchive &);
template void engine::Scene::serialize<cereal::PortableBinaryOutputArchive>(cereal::PortableBinaryOutputArchive &);
template void engine::Scene::serialize<cereal::PortableBinaryInputArchive>(cereal::PortableBinaryInputArchive &);

CEREAL_REGISTER_TYPE(engine::Scene)