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
    assert(FALSE && "not implemented");
    // TODO: implement scene destroy
}
}