#include "scene.h"

#include "game_object.h"

namespace engine
{
void Scene::OnFixedUpdate()
{
    for (const auto &game_object : m_root_game_objects_)
    {
        game_object->InvokeFixedUpdate();
    }
}

void Scene::OnUpdate()
{
    for (const auto &game_object : m_root_game_objects_)
    {
        game_object->InvokeUpdate();
    }
}

const std::vector<std::shared_ptr<GameObject>> &Scene::RootGameObjects()
{
    return m_root_game_objects_;
}
}