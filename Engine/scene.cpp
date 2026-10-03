#include "pch.h"

#include "scene.h"

#include "application.h"
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
        game_object->m_scene_ = self;
        if (!Application::IsPlayMode())
        {
            game_object->InvokeOnValidate();
        }
    }
}

void Scene::OnDeserialized()
{
    const auto self = shared_from_base<Scene>();

    for (const auto &game_object : m_all_game_objects_)
    {
        game_object->m_scene_ = self;
        if (!Application::IsPlayMode())
        {
            game_object->InvokeOnValidate();
        }
    }
}

void Scene::OnUpdate()
{
    const auto root_objects = m_root_game_objects_;
    for (const auto &game_object : root_objects)
    {
        game_object->InvokeOnUpdate();
    }
}

void Scene::OnFixedUpdate()
{
    for (const auto &game_object : m_root_game_objects_)
    {
        game_object->InvokeOnFixedUpdate();
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
        const auto is_destroying = [](const auto &go) {
            return go->IsDestroying();
        };
        std::erase_if(m_root_game_objects_, is_destroying);
        std::erase_if(m_all_game_objects_, is_destroying);
        m_has_destroying_game_object_ = false;
    }
}

const std::vector<std::shared_ptr<GameObject>> &Scene::RootGameObjects()
{
    return m_root_game_objects_;
}

const std::vector<std::shared_ptr<GameObject>> &Scene::AllGameObjects()
{
    return m_all_game_objects_;
}

void Scene::MarkDestroyingGameObject()
{
    m_has_destroying_game_object_ = true;
}

void Scene::MoveGameObject(const std::shared_ptr<GameObject> &go)
{
    const auto this_scene = shared_from_base<Scene>();
    const auto prev_scene = go->Scene();

    go->m_scene_ = this_scene;

    // handle ownership change 
    if (prev_scene != this_scene && prev_scene != nullptr)
    {
        erase_if(
            prev_scene->m_all_game_objects_,
            [&go](const auto &a) {
                return a == go;
            }
        );
        erase_if(
            prev_scene->m_root_game_objects_,
            [&go](const auto &a) {
                return a == go;
            }
        );
    }

    // refresh current status
    const auto is_root_object = go->Transform() == nullptr || go->Transform()->Parent() == nullptr;
    const auto root_pos = std::ranges::find(m_root_game_objects_, go);
    const auto all_pos = std::ranges::find(m_all_game_objects_, go);

    // update all object list
    if (all_pos == m_all_game_objects_.end())
    {
        m_all_game_objects_.emplace_back(go);
    }

    // update root object list
    if (is_root_object)
    {
        if (root_pos == m_root_game_objects_.end())
        {
            m_root_game_objects_.emplace_back(go);
        }
    }
    else
    {
        erase_if(
            m_root_game_objects_,
            [&go](const auto &a) {
                return a == go;
            }
        );
    }

    // update child objects
    const auto transform = go->Transform();
    if (transform != nullptr)
    {
        // if parent is not moving to this scene, then we'll have to detach the current object
        if (transform->Parent() != nullptr && transform->Parent()->GameObject()->Scene() != this_scene)
        {
            transform->SetParent(nullptr);
        }

        for (int i = 0; i < transform->ChildCount(); ++i)
        {
            MoveGameObject(transform->GetChild(i)->GameObject());
        }
    }
}

void Scene::ReorderRootObject(std::shared_ptr<GameObject> target_object, const int dst_idx)
{
    const auto result = std::erase(m_root_game_objects_, target_object);
    if (result == 0)
    {
        Logger::Error<Scene>("ReorderRootObject: Could not find target object in RootObjects list");
        return;
    }

    if (dst_idx <= 0)
    {
        m_root_game_objects_.insert(m_root_game_objects_.begin(), target_object);
        return;
    }

    if (m_root_game_objects_.size() < dst_idx)
    {
        m_root_game_objects_.emplace_back(target_object);
        return;
    }

    m_root_game_objects_.insert(m_root_game_objects_.begin() + dst_idx, target_object);
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