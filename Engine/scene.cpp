#include "pch.h"

#include "scene.h"

#include "game_object.h"
#include "update_manager.h"

#include "cereal/archives/json.hpp"

namespace engine
{
void Scene::OnConstructed()
{
    UpdateManager::SubscribeUpdate(shared_from_base<Scene>());
    UpdateManager::SubscribeFixedUpdate(shared_from_base<Scene>());
}
void Scene::OnUpdate()
{
    if (!m_destroying_components_.empty())
    {
        Logger::Log<Scene>("Destroying %d components", m_destroying_components_.size());
        for (const auto &component : m_destroying_components_)
        {
            component->OnDestroy();
            const auto go = component->GameObject();
            go->m_components_.erase(std::ranges::find(component->GameObject()->m_components_, component));
        }

        m_destroying_components_.clear();
    }

    if (!m_destroying_objects_.empty())
    {
        Logger::Log<Scene>("Destroying %d game objects", m_destroying_objects_.size());
        for (const auto &game_object : m_destroying_objects_)
        {
            game_object->OnDestroy();
        }

        for (const auto &game_object : m_destroying_objects_)
        {
            const auto all_obj_it = std::ranges::find(m_all_game_objects_, game_object);
            if (all_obj_it != m_all_game_objects_.end())
                m_all_game_objects_.erase(all_obj_it);

            const auto root_obj_it = std::ranges::find(m_root_game_objects_, game_object);
            if (root_obj_it != m_root_game_objects_.end())
                m_root_game_objects_.erase(root_obj_it);
        }

        m_destroying_objects_.clear();
    }

    auto buff = m_root_game_objects_;
    for (const auto &game_object : buff)
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
void Scene::MarkDestroying(const std::shared_ptr<GameObject> &game_object)
{
    m_destroying_objects_.emplace(game_object);
    const auto transform = game_object->Transform();
    for (int i = 0; i < transform->ChildCount(); i++)
    {
        MarkDestroying(transform->GetChild(i)->GameObject());
    }
}
void Scene::MarkDestroying(const std::shared_ptr<Component> &component)
{
    m_destroying_components_.emplace(component);
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