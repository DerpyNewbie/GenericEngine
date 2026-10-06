#pragma once
#include <memory>
#include "game_object.h"
#include "editor_window.h"

namespace editor
{
/// <summary>
/// HierarchyのGuiを管理するクラス
/// </summary>
class Hierarchy final : public EditorWindow
{
public:
    /// <summary>
    /// このWindowの名前を返します。Windowのタイトルとして使われます。
    /// </summary>
    /// <returns>Windowの名前</returns>
    std::string Name() override;
    /// <summary>
    /// 現在読み込まれているすべてのSceneとその中のGameObjectをGuiに表示します。
    /// </summary>
    void OnEditorGui() override;

private:
    /// <summary>
    /// Sceneを表示します。SceneのHeaderがクリックされている時Sceneの中にあるObjectも表示します。
    /// </summary>
    /// <param name="scene">表示されるScene</param>
    void DrawScene(const std::shared_ptr<engine::Scene> &scene);

    /// <summary>
    /// ObjectのChildがなくなるまで再帰的に表示します。
    /// </summary>
    /// <param name="game_object">表示されるGameObject</param>
    void DrawObjectRecursive(const std::shared_ptr<engine::GameObject> &game_object);
    /// <summary>
    /// Objectを表示します。
    /// </summary>
    /// <param name="game_object">表示されるGameObject</param>
    /// <returns></returns>
    bool DrawObject(const std::shared_ptr<engine::GameObject> &game_object);
    /// <summary>
    /// Objectの並び替え用のDragDropTargetを表示します。Dropされた場合、DropされたObjectを指定されたObjectと同じ親の子にし、並び順を変更します。
    /// </summary>
    /// <param name="game_object">並び替えの基準となるGameObject</param>
    /// <param name="offset">基準となるGameObjectのSiblingIndexからのずれ。 0 : 前, 1 : 後ろ</param>
    void DrawReorderingTarget(const std::shared_ptr<engine::GameObject> &game_object, int offset);
};
}