#include "pch.h"

#include "scene_manager.h"
#include "scene.h"

namespace engine
{
std::vector<std::shared_ptr<Scene>> SceneManager::m_scenes_;

std::shared_ptr<Scene> SceneManager::GetActiveScene()
{
    return m_scenes_[0];
}

const std::vector<std::shared_ptr<Scene>> &SceneManager::GetCurrentScenes()
{
    return m_scenes_;
}

std::shared_ptr<Scene> SceneManager::CreateScene(const std::string &name)
{
    auto scene = Object::Instantiate<Scene>(name);
    m_scenes_.push_back(scene);
    return scene;
}

void SceneManager::AddScene(const std::shared_ptr<Scene> &scene)
{
    m_scenes_.push_back(scene);
}

void SceneManager::DestroyScene(const std::string &name)
{
    auto pos = std::ranges::find_if(m_scenes_, [&](std::shared_ptr<Scene> scene) {
        return scene->Name() == name;
    });
    if (pos == m_scenes_.end())
        return;

    Object::Destroy(*pos);
    m_scenes_.erase(pos);
}

void SceneManager::MoveGameObject(const std::shared_ptr<GameObject> &go, const std::shared_ptr<Scene> &scene)
{
    if (!go->m_scene_.expired() && go->Transform()->Parent() == nullptr)
    {
        go->SetAsRootObject(false);
    }

    if (const auto prev_scene = go->m_scene_.lock())
    {
        const auto pos = std::ranges::find(prev_scene->m_all_game_objects_, go);
        if (pos != prev_scene->m_all_game_objects_.end())
            prev_scene->m_all_game_objects_.erase(pos);
    }

    go->m_scene_ = scene;
    scene->m_all_game_objects_.emplace_back(go);
    if (go->Transform()->Parent() == nullptr)
    {
        go->SetAsRootObject(true);
    }
}
}