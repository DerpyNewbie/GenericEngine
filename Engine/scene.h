#pragma once
#include "event_receivers.h"
#include "object.h"

#include <memory>
#include <vector>

namespace engine
{
class GameObject;
class final Scene : public Object, public IUpdateReceiver, public IFixedUpdateReceiver
{
    friend class GameObject;
    std::vector<std::shared_ptr<GameObject>> m_root_game_objects_;

    void OnUpdate() override;
    void OnFixedUpdate() override;
public:
    const std::vector<std::shared_ptr<GameObject>>& RootGameObjects();

};
}