#pragma once
#include "game_object.h"

namespace editor
{
/// <summary>
/// Editor用のGizmos(Gridや選択中のObjectなど)を描画するクラスです。
/// </summary>
class EditorGizmos
{
public:
    /// <summary>
    /// Y 0 地点にGrid状の床を表示させます。
    /// </summary>
    /// <param name="view_matrix">カメラのview_matrix</param>
    /// <param name="spacing">Gridの幅</param>
    /// <param name="count">Grid 1列の数</param>
    /// <remark>
    /// めちゃくちゃ重たいです。
    /// </remark>
    static void DrawYPlaneGrid(const Matrix &view_matrix = Matrix::Identity,
                               const Vector2 &spacing = {1, 1}, int count = 50);

    /// <summary>
    /// Objectの座標にSphereを表示させます。
    /// </summary>
    /// <param name="game_object">対象のObject</param>
    static void DrawObject(const std::shared_ptr<engine::GameObject> &game_object);
};
}