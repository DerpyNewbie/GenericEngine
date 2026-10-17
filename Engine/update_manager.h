#pragma once
#include "event_receivers.h"

namespace engine
{
/// <summary>
/// Update、FixedUpdate、Render、GarbageCollectを受け取るReceiverを管理し、呼び出すクラスです。
/// </summary>
class UpdateManager
{
    friend class Engine;

    inline static std::vector<std::weak_ptr<IUpdateReceiver>> m_update_receivers_;
    inline static std::vector<std::weak_ptr<IFixedUpdateReceiver>> m_fixed_update_receivers_;
    inline static std::vector<std::weak_ptr<IRenderReceiver>> m_render_receivers_;
    inline static std::vector<std::weak_ptr<IGarbageCollectReceiver>> m_garbage_collect_receivers_;
    inline static std::queue<std::function<void()>> m_in_cycle_buffer_;

    inline static bool m_in_update_cycle_;
    inline static bool m_in_fixed_update_cycle_;
    inline static bool m_in_render_cycle_;
    inline static bool m_in_garbage_collect_cycle_;

    /// <summary>
    /// 登録されているすべてのReceiverのOnUpdateを呼び出します。
    /// </summary>
    static void InvokeUpdate();
    /// <summary>
    /// 登録されているすべてのReceiverのOnFixedUpdateを呼び出します。
    /// </summary>
    static void InvokeFixedUpdate();
    /// <summary>
    /// 登録されているすべてのReceiverのRenderを呼び出します。
    /// </summary>
    static void InvokeRender();
    /// <summary>
    /// 登録されているすべてのReceiverのOnGarbageCollectを呼び出します。
    /// </summary>
    static void InvokeGarbageCollect();
    /// <summary>
    /// 呼び出しの最中に予約された登録 / 解除の処理を実行します。
    /// </summary>
    static void PostFix();

public:
    /// <summary>
    /// Updateを受け取るReceiverを登録します。Updateの最中に呼ばれた場合は、終了後に登録されます。
    /// </summary>
    /// <param name="receiver">登録するReceiver</param>
    static void SubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver);
    /// <summary>
    /// Updateを受け取るReceiverの登録を解除します。Updateの最中に呼ばれた場合は、終了後に解除されます。
    /// </summary>
    /// <param name="receiver">解除するReceiver</param>
    static void UnsubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver);
    /// <summary>
    /// FixedUpdateを受け取るReceiverを登録します。FixedUpdateの最中に呼ばれた場合は、終了後に登録されます。
    /// </summary>
    /// <param name="receiver">登録するReceiver</param>
    static void SubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver);
    /// <summary>
    /// FixedUpdateを受け取るReceiverの登録を解除します。FixedUpdateの最中に呼ばれた場合は、終了後に解除されます。
    /// </summary>
    /// <param name="receiver">解除するReceiver</param>
    static void UnsubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver);
    /// <summary>
    /// Renderを受け取るReceiverを登録します。Renderの最中に呼ばれた場合は、終了後に登録されます。
    /// </summary>
    /// <param name="receiver">登録するReceiver</param>
    static void SubscribeRender(const std::shared_ptr<IRenderReceiver> &receiver);
    /// <summary>
    /// Renderを受け取るReceiverの登録を解除します。Renderの最中に呼ばれた場合は、終了後に解除されます。
    /// </summary>
    /// <param name="receiver">解除するReceiver</param>
    static void UnsubscribeRender(const std::shared_ptr<IRenderReceiver> &receiver);
    /// <summary>
    /// GarbageCollectを受け取るReceiverを登録します。GarbageCollectの最中に呼ばれた場合は、終了後に登録されます。
    /// </summary>
    /// <param name="receiver">登録するReceiver</param>
    static void SubscribeGarbageCollect(const std::shared_ptr<IGarbageCollectReceiver> &receiver);
    /// <summary>
    /// GarbageCollectを受け取るReceiverの登録を解除します。GarbageCollectの最中に呼ばれた場合は、終了後に解除されます。
    /// </summary>
    /// <param name="receiver">解除するReceiver</param>
    static void UnsubscribeGarbageCollect(const std::shared_ptr<IGarbageCollectReceiver> &receiver);

    /// <summary>
    /// 登録されているUpdateのReceiverの数を取得します。
    /// </summary>
    static int UpdateCount()
    {
        return static_cast<int>(m_update_receivers_.size());
    }

    /// <summary>
    /// 登録されているFixedUpdateのReceiverの数を取得します。
    /// </summary>
    static int FixedUpdateCount()
    {
        return static_cast<int>(m_fixed_update_receivers_.size());
    }

    /// <summary>
    /// 登録されているUpdateのReceiverのリストを取得します。
    /// </summary>
    static const std::vector<std::weak_ptr<IUpdateReceiver>> &GetUpdateReceivers()
    {
        return m_update_receivers_;
    }

    /// <summary>
    /// 登録されているFixedUpdateのReceiverのリストを取得します。
    /// </summary>
    static const std::vector<std::weak_ptr<IFixedUpdateReceiver>> &GetFixedUpdateReceivers()
    {
        return m_fixed_update_receivers_;
    }

    /// <summary>
    /// Updateの呼び出しの最中であるかどうかを取得します。
    /// </summary>
    static bool InUpdateCycle();
    /// <summary>
    /// FixedUpdateの呼び出しの最中であるかどうかを取得します。
    /// </summary>
    static bool InFixedUpdateCycle();
    /// <summary>
    /// Renderの呼び出しの最中であるかどうかを取得します。
    /// </summary>
    static bool InRenderCycle();
    /// <summary>
    /// GarbageCollectの呼び出しの最中であるかどうかを取得します。
    /// </summary>
    static bool InGarbageCollectCycle();
};
}