#pragma once
#include "cancellation_token.h"
#include "task.h"

namespace engine
{
//先人がゲームとエンジンのcoroutineは別けたほうがいいよって言ってたのでシングルトンじゃないです
class CoroutineManager
{
    using CoroutineEntry = std::pair<std::coroutine_handle<Task::promise_type>, CancellationToken>;
    std::vector<CoroutineEntry> m_coroutines_;

public:
    /// <summary>
    /// 
    /// </summary>
    /// <param name="t">タスク</param>
    void Start(Task &&t);
    /// <summary>
    /// coroutineを開始します。
    /// </summary>
    /// <param name="t">Task</param>
    /// <param name="token">CancellationToken</param>
    void Start(Task &&t, CancellationToken token);

    /// <summary>
    /// coroutineのupdateを行います。
    /// </summary>
    void Update();
};
}