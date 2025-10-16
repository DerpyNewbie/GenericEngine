#pragma once
#include "game_object.h"

namespace editor
{
class EditorGizmos
{
public:
    static void DrawYPlaneGrid(const Matrix &view_matrix = Matrix::Identity,
                               const Vector2 &spacing = {1, 1}, int count = 50);

    static void DrawObject(const std::shared_ptr<engine::GameObject> &game_object);
};
}