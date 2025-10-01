#pragma once
#include "event_receivers.h"

namespace engine
{
class Scene : public Object, public IUpdateReceiver, public IFixedUpdateReceiver
{
    friend class ObjectUtil;
    friend class GameObject;
    friend class SceneManager;
    friend class Transform;

    std::vector<std::shared_ptr<GameObject>> m_root_game_objects_;
    std::vector<std::shared_ptr<GameObject>> m_all_game_objects_;
    bool m_has_destroying_game_object_ = false;

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