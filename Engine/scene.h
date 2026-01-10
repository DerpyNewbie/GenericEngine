#pragma once
#include "event_receivers.h"

namespace engine
{
class GameObject;
class Scene : public Object, public IUpdateReceiver, public IFixedUpdateReceiver, public IGarbageCollectReceiver
{
    friend class SceneManager;

    std::vector<std::shared_ptr<GameObject>> m_root_game_objects_;
    std::vector<std::shared_ptr<GameObject>> m_all_game_objects_;
    bool m_has_destroying_game_object_ = false;

    void OnConstructed() override;
    void OnDeserialized() override;
    void OnUpdate() override;
    void OnFixedUpdate() override;
    void OnDestroy() override;
    void OnGarbageCollect() override;

public:
    const std::vector<std::shared_ptr<GameObject>> &RootGameObjects();
    const std::vector<std::shared_ptr<GameObject>> &AllGameObjects();

    void MarkDestroyingGameObject();
    void MoveGameObject(const std::shared_ptr<GameObject> &go);
    void ReorderRootObject(std::shared_ptr<GameObject> target_object, int dst_idx);

    template <class Archive>
    void serialize(Archive &ar, uint32_t version);
};
}

CEREAL_CLASS_VERSION(engine::Scene, 1)