#include "scene_manager.h"

#include "scene.h"

#include <cassert>

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

void SceneManager::DestroyScene(const std::string &name)
{
    auto pos = std::ranges::find_if(m_scenes_, [&](std::shared_ptr<Scene> scene) {
        return scene->Name() == name;
    });
    if (pos == m_scenes_.end())
        return;

    m_scenes_.erase(pos);
}

void SceneManager::MoveGameObject(const std::shared_ptr<GameObject> &go, const std::shared_ptr<Scene> &scene)
{
    if (!go->m_scene_.expired() && go->Transform()->Parent() == nullptr)
    {
        go->SetAsRootObject(false);
    }

    go->m_scene_ = scene;
    if (go->Transform()->Parent() == nullptr)
    {
        go->SetAsRootObject(true);
    }
}
}