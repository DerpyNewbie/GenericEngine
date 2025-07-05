#pragma once
#include "object.h"
#include "event_receivers.h"
#include "Components/component.h"

#include <memory>
#include <vector>

namespace engine
{
class Scene : public Object, public IUpdateReceiver, public IFixedUpdateReceiver
{
    friend class GameObject;
    friend class SceneManager;
    std::vector<std::shared_ptr<GameObject>> m_root_game_objects_;
    std::vector<std::shared_ptr<GameObject>> m_all_game_objects_;

    void OnConstructed() override;
    void OnUpdate() override;
    void OnFixedUpdate() override;
    void OnDestroy() override;

public:
    const std::vector<std::shared_ptr<GameObject>> &RootGameObjects();

    template <class Archive>
    void serialize(Archive &ar);
};
}