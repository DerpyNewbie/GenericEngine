#include "pch.h"

#include "scene.h"

#include "game_object.h"
#include "update_manager.h"

namespace engine
{
void Scene::OnConstructed()
{
    const auto self = shared_from_base<Scene>();
    UpdateManager::SubscribeUpdate(self);
    UpdateManager::SubscribeFixedUpdate(self);
    UpdateManager::SubscribeGarbageCollect(self);

    for (const auto &game_object : m_all_game_objects_)
    {
        game_object->InvokeOnValidate();
    }
}

void Scene::OnUpdate()
{
    const auto root_objects = m_root_game_objects_;
    for (const auto &game_object : root_objects)
    {
        game_object->InvokeUpdate();
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
    const auto self = shared_from_base<Scene>();
    UpdateManager::UnsubscribeUpdate(self);
    UpdateManager::UnsubscribeFixedUpdate(self);
    UpdateManager::UnsubscribeGarbageCollect(self);
    for (const auto &game_object : m_root_game_objects_)
    {
        Destroy(game_object);
    }
}

void Scene::OnGarbageCollect()
{
    if (m_has_destroying_game_object_)
    {
        std::erase_if(
            m_root_game_objects_,
            [](const auto &go) {
                return go->IsDestroying();
            }
        );

        std::erase_if(
            m_all_game_objects_,
            [](const auto &go) {
                return go->IsDestroying();
            }
        );
        m_has_destroying_game_object_ = false;
    }
}

const std::vector<std::shared_ptr<GameObject>> &Scene::RootGameObjects()
{
    return m_root_game_objects_;
}

template <class Archive>
void Scene::serialize(Archive &ar, const uint32_t version)
{
    ar(cereal::base_class<Object>(this), CEREAL_NVP(m_root_game_objects_), CEREAL_NVP(m_all_game_objects_));
}
}

template void engine::Scene::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive &, uint32_t);
template void engine::Scene::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive &, uint32_t);
template void engine::Scene::serialize<cereal::XMLOutputArchive>(cereal::XMLOutputArchive &, uint32_t);
template void engine::Scene::serialize<cereal::XMLInputArchive>(cereal::XMLInputArchive &, uint32_t);
template void engine::Scene::serialize<cereal::PortableBinaryOutputArchive>(cereal::PortableBinaryOutputArchive &, uint32_t);
template void engine::Scene::serialize<cereal::PortableBinaryInputArchive>(cereal::PortableBinaryInputArchive &, uint32_t);

CEREAL_REGISTER_TYPE(engine::Scene)