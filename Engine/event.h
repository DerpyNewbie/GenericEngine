#pragma once

namespace engine
{
/// <summary>
/// Listenerを登録して、まとめて呼び出せるEventです。
/// </summary>
template <class... ArgTypes>
class Event
{
    using ListenerToken = size_t;
    std::unordered_map<ListenerToken, std::function<void(ArgTypes...)>> m_listeners_;
    ListenerToken m_next_listener_token_ = 0;

public:
    /// <summary>
    /// Listenerを登録します。
    /// </summary>
    /// <param name="callback">Invokeされた時に呼ばれる関数</param>
    /// <returns>RemoveListenerで使うToken</returns>
    ListenerToken AddListener(std::function<void(ArgTypes...)> callback)
    {
        auto token = ++m_next_listener_token_;
        m_listeners_.emplace(token, callback);
        return token;
    }

    /// <summary>
    /// Listenerの登録を解除します。
    /// </summary>
    /// <param name="token">AddListenerの戻り値</param>
    void RemoveListener(ListenerToken token)
    {
        m_listeners_.erase(token);
    }

    /// <summary>
    /// 登録されているすべてのListenerのコピーを取得します。
    /// </summary>
    std::unordered_map<ListenerToken, std::function<void(ArgTypes...)>> Listeners()
    {
        return m_listeners_;
    }

    /// <summary>
    /// 登録されているすべてのListenerを呼び出します。
    /// </summary>
    /// <param name="args">Listenerに渡す引数</param>
    void Invoke(ArgTypes... args)
    {
        for (auto &callback : m_listeners_ | std::views::values)
            callback(args...);
    }

    /// <summary>
    /// すべてのListenerの登録を解除します。
    /// </summary>
    void Clear()
    {
        m_listeners_.clear();
    }
};
}