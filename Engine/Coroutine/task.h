#pragma once
#include <utility>

#include "wait_for_frames.h"
#include "wait_for_next_frame.h"
#include "wait_for_seconds.h"
#include "wait_for_task.h"
#include "yield_base.h"

namespace engine
{
/// <summary>
/// Coroutineとして実行できるTaskです。
/// </summary>
struct Task
{
    /// <summary>
    /// C++のCoroutineがTaskを扱うために必要なpromise_typeです。
    /// </summary>
    struct promise_type
    {
        bool completed = false;
        std::vector<std::coroutine_handle<>> waiters;
        std::unique_ptr<YieldBase> current_yield = nullptr;

        /// <summary>
        /// Coroutineの戻り値となるTaskを生成します。
        /// </summary>
        Task get_return_object()
        {
            return Task(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        /// <summary>
        /// Coroutineを開始時に中断させます。再開されるまで本体は実行されません。
        /// </summary>
        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }
        /// <summary>
        /// Coroutineを終了時に中断させます。handleは自動では破棄されません。
        /// </summary>
        std::suspend_always final_suspend() noexcept
        {
            return {};
        }
        /// <summary>
        /// co_returnされた時に呼ばれます。何もしません。
        /// </summary>
        void return_void()
        {}
        /// <summary>
        /// Coroutineの中で捕捉されなかった例外をログに出力します。
        /// </summary>
        void unhandled_exception()
        {
            Logger::Error<Task>("Coroutine exception: {}", std::current_exception());
        }

        /// <summary>
        /// co_awaitされたWaitForNextFrameをcurrent_yieldに保存し、待機条件として使います。
        /// </summary>
        auto await_transform(WaitForNextFrame w)
        {
            current_yield = std::make_unique<WaitForNextFrame>(std::move(w));
            return *static_cast<WaitForNextFrame *>(current_yield.get());
        }

        /// <summary>
        /// co_awaitされたWaitForSecondsをcurrent_yieldに保存し、待機条件として使います。
        /// </summary>
        auto await_transform(WaitForSeconds w)
        {
            current_yield = std::make_unique<WaitForSeconds>(std::move(w));
            return *static_cast<WaitForSeconds *>(current_yield.get());
        }
        /// <summary>
        /// co_awaitされたWaitForFramesをcurrent_yieldに保存し、待機条件として使います。
        /// </summary>
        auto await_transform(WaitForFrames w)
        {
            current_yield = std::make_unique<WaitForFrames>(std::move(w));
            return *static_cast<WaitForFrames *>(current_yield.get());
        }

        /// <summary>
        /// co_awaitされたWaitForTaskをcurrent_yieldに保存し、待機条件として使います。
        /// </summary>
        auto await_transform(WaitForTask w)
        {
            current_yield = std::make_unique<WaitForTask>(std::move(w));
            return *static_cast<WaitForTask *>(current_yield.get());
        }
    };

    std::coroutine_handle<promise_type> handle;

    /// <summary>
    /// Coroutineのhandleを所有するTaskを作成します。
    /// </summary>
    explicit Task(std::coroutine_handle<promise_type> h) : handle(h)
    {}
    /// <summary>
    /// Moveコンストラクタです。handleの所有権を移します。
    /// </summary>
    Task(Task &&t) noexcept : handle(t.handle)
    {
        t.handle = {};
    }
    /// <summary>
    /// handleを所有している場合、Coroutineを破棄します。
    /// </summary>
    ~Task()
    {
        if (handle)
            handle.destroy();
    }
};
}