#pragma once
#include "Coroutine/task.h"

namespace engine
{
class Do
{
public:
    static Task Transformation(const std::shared_ptr<Transform> &transform, const Matrix &to, float duration);
    static Task Move(std::shared_ptr<Transform> transform, const Vector3 &to, float duration);
    static Task Rotation(std::shared_ptr<Transform> transform, const Quaternion &to, float duration);
    static Task Scale(std::shared_ptr<Transform> transform, const Vector3 &to, float duration);
};
}