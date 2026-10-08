#pragma once
#include "Components/transform.h"
#include "Coroutine/task.h"

namespace engine
{
/// <summary>
/// Transformを一定時間かけて移動、回転、拡縮させるCoroutine(Task)を提供するクラスです。
/// </summary>
class Do
{
public:
    /// <summary>
    /// Transformを指定された時間をかけて目標のMatrixまで補間するCoroutineです。
    /// </summary>
    /// <param name="transform">対象のTransform</param>
    /// <param name="to">目標のMatrix(World空間)</param>
    /// <param name="duration">かける時間(秒)</param>
    static Task Transformation(const std::shared_ptr<Transform> &transform, const Matrix &to, float duration);
    /// <summary>
    /// TransformのWorld座標を指定された時間をかけて目標の座標まで線形補間するCoroutineです。
    /// </summary>
    /// <param name="transform">対象のTransform</param>
    /// <param name="to">目標のWorld座標</param>
    /// <param name="duration">かける時間(秒)</param>
    static Task Move(std::shared_ptr<Transform> transform, const Vector3 &to, float duration);
    /// <summary>
    /// TransformのWorld回転を指定された時間をかけて目標の回転まで球面線形補間するCoroutineです。
    /// </summary>
    /// <param name="transform">対象のTransform</param>
    /// <param name="to">目標のWorld回転</param>
    /// <param name="duration">かける時間(秒)</param>
    static Task Rotation(std::shared_ptr<Transform> transform, const Quaternion &to, float duration);
    /// <summary>
    /// TransformのScaleを指定された時間をかけて目標のScaleまで線形補間するCoroutineです。親のScaleで割った値をLocalScaleに設定します。
    /// </summary>
    /// <param name="transform">対象のTransform</param>
    /// <param name="to">目標のWorld Scale</param>
    /// <param name="duration">かける時間(秒)</param>
    static Task Scale(std::shared_ptr<Transform> transform, const Vector3 &to, float duration);
};
}