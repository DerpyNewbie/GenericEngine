#pragma once
#include "game_object.h"

#include <memory>
#include <vector>

namespace engine
{
class SceneManager
{
    friend class Editor;
    static std::vector<std::shared_ptr<Scene>> m_scenes_;

public:
    static std::shared_ptr<Scene> GetActiveScene();
    static const std::vector<std::shared_ptr<Scene>> &GetCurrentScenes();
    static std::shared_ptr<Scene> CreateScene(const std::string &name);
    static void DestroyScene(const std::string &name);
    static void MoveGameObject(const std::shared_ptr<GameObject> &go, const std::shared_ptr<Scene> &scene);
};
}