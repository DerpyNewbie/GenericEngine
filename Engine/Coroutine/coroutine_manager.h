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
    /// 指定されたTaskをリストに追加します。
    /// </summary>
    /// <param name="t">Task</param>
    void Start(Task &&t);
    /// <summary>
    /// 指定されたTaskをリストに追加します。
    /// </summary>
    /// <param name="t">Task</param>
    /// <param name="token">CancellationToken</param>
    void Start(Task &&t, CancellationToken token);

    /// <summary>
    /// coroutineが再開可能であればawaitに当たるまで進めます。Cancel もしくは co_return に当たった場合はリストから削除されます。
    /// </summary>
    /// <remarks>
    /// 任意updateのタイミングで一度だけ呼び出します。
    /// </remarks>
    void Update();
};
}