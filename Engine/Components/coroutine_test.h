#pragma once
#include "component.h"
#include "Coroutine/task.h"

namespace engine
{
/// <summary>
/// Coroutineの動作確認用のComponentです。
/// </summary>
class CoroutineTest : public Component
{
    /// <summary>
    /// LocalPositionのxが 100 に達するまで、毎フレーム 0.1 ずつ移動させるCoroutineです。
    /// </summary>
    Task Move();
    /// <summary>
    /// Moveの完了を待ってからログを出力するCoroutineです。
    /// </summary>
    Task MoveWrap();

public:
    /// <summary>
    /// Do::Moveを使い、10 秒かけて (100, 0, 0) まで移動するCoroutineを開始します。
    /// </summary>
    void OnAwake() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Component>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::CoroutineTest, 1)