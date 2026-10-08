#pragma once
#include "event_receivers.h"

namespace engine
{
class GameObject;
/// <summary>
/// GameObjectをまとめて管理するSceneです。
/// </summary>
class Scene : public Object, public IUpdateReceiver, public IFixedUpdateReceiver, public IGarbageCollectReceiver
{
    friend class SceneManager;

    std::vector<std::shared_ptr<GameObject>> m_root_game_objects_;
    std::vector<std::shared_ptr<GameObject>> m_all_game_objects_;
    bool m_has_destroying_game_object_ = false;

    /// <summary>
    /// UpdateManagerに自身を登録し、持っているすべてのGameObjectに自身をSceneとして設定します。
    /// </summary>
    void OnConstructed() override;
    /// <summary>
    /// 持っているすべてのGameObjectに自身をSceneとして設定します。
    /// </summary>
    void OnDeserialized() override;
    void OnUpdate() override;
    void OnFixedUpdate() override;
    /// <summary>
    /// UpdateManagerへの登録を解除し、RootのGameObjectをすべて破棄します。
    /// </summary>
    void OnDestroy() override;
    /// <summary>
    /// 破棄待ちのGameObjectをリストから取り除きます。
    /// </summary>
    void OnGarbageCollect() override;

public:
    /// <summary>
    /// 親を持たないGameObjectのリストを取得します。
    /// </summary>
    const std::vector<std::shared_ptr<GameObject>> &RootGameObjects();
    /// <summary>
    /// このSceneに属するすべてのGameObjectのリストを取得します。
    /// </summary>
    const std::vector<std::shared_ptr<GameObject>> &AllGameObjects();

    /// <summary>
    /// 破棄されるGameObjectがあることを記録します。次のOnGarbageCollectでリストから取り除かれます。
    /// </summary>
    void MarkDestroyingGameObject();
    /// <summary>
    /// GameObjectとその子をこのSceneに移動します。親が別のSceneにある場合は親から切り離します。
    /// </summary>
    /// <param name="go">移動するGameObject</param>
    void MoveGameObject(const std::shared_ptr<GameObject> &go);
    /// <summary>
    /// RootのGameObjectの並び順を変更します。
    /// </summary>
    /// <param name="target_object">移動するGameObject</param>
    /// <param name="dst_idx">移動先のindex</param>
    void ReorderRootObject(std::shared_ptr<GameObject> target_object, int dst_idx);

    template <class Archive>
    void serialize(Archive &ar, uint32_t version);
};
}

CEREAL_CLASS_VERSION(engine::Scene, 1)