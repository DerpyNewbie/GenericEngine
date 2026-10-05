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
    /// coroutineをリストに追加します。
    /// </summary>
    /// <param name="t">task</param>
    void Start(Task &&t);
    /// <summary>
    /// coroutineをリストに追加します。
    /// </summary>
    /// <param name="t">task</param>
    /// <param name="token">CancellationToken</param>
    void Start(Task &&t, CancellationToken token);

    /// <summary>
    /// coroutineを再開可能であれば再開します。cancelもしくはco_returnされた場合リストから削除します。
    /// </summary>
    /// <remarks>
    /// Engine側のUpdateサイクルのタイミングで1度だけ呼び出してください。
    /// </remarks>
    void Update();
};
}