#pragma once
#include "event.h"
#include "game_object.h"

namespace engine
{
/// <summary>
/// Sceneの作成、追加、破棄と、現在のSceneの管理を行うクラスです。
/// </summary>
class SceneManager
{
    friend class Editor;
    inline static std::vector<std::shared_ptr<Scene>> m_scenes_;
    inline static bool m_is_deserializing_scene_;

public:
    inline static Event<std::shared_ptr<Scene>> scene_added;

    /// <summary>
    /// ActiveなScene(リストの先頭のScene)を取得します。
    /// </summary>
    /// <returns>Sceneが1つもない場合 nullptr</returns>
    static std::shared_ptr<Scene> GetActiveScene();
    /// <summary>
    /// 現在読み込まれているすべてのSceneを取得します。
    /// </summary>
    static const std::vector<std::shared_ptr<Scene>> &GetCurrentScenes();
    /// <summary>
    /// 新しいSceneを作成し、リストに追加します。
    /// </summary>
    /// <param name="name">Sceneの名前</param>
    static std::shared_ptr<Scene> CreateScene(const std::string &name);
    /// <summary>
    /// scene_addedを呼び出し、Sceneをリストに追加します。
    /// </summary>
    /// <param name="scene">追加するScene</param>
    static void AddScene(const std::shared_ptr<Scene> &scene);
    /// <summary>
    /// Serializeされた文字列からSceneを復元し、リストに追加します。
    /// </summary>
    /// <param name="serialized_scene">SerializeされたScene</param>
    static void DeserializeScene(const std::string &serialized_scene);
    /// <summary>
    /// 指定された名前のSceneを破棄し、リストから取り除きます。
    /// </summary>
    /// <param name="name">Sceneの名前</param>
    static void DestroyScene(const std::string &name);
    /// <summary>
    /// GameObjectを指定されたSceneに移動します。SceneのDeserialize中は何もしません。
    /// </summary>
    /// <param name="go">移動するGameObject</param>
    /// <param name="scene">移動先のScene</param>
    static void MoveGameObject(const std::shared_ptr<GameObject> &go, const std::shared_ptr<Scene> &scene);
};
}