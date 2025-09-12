#pragma once

namespace engine
{
template <class... ArgTypes>
class Event
{
    using ListenerToken = size_t;
    std::unordered_map<ListenerToken, std::function<void(ArgTypes...)>> m_listeners_;
    ListenerToken m_next_listener_token_ = 0;

public:
    ListenerToken AddListener(std::function<void(ArgTypes...)> callback)
    {
        auto token = ++m_next_listener_token_;
        m_listeners_.emplace(token, callback);
        return token;
    }

    void RemoveListener(ListenerToken token)
    {
        m_listeners_.erase(token);
    }

    std::unordered_map<ListenerToken, std::function<void(ArgTypes...)>> Listeners()
    {
        return m_listeners_;
    }

    void Invoke(ArgTypes... args)
    {
        for (auto &callback : m_listeners_ | std::views::values)
            callback(args...);
    }

    void Clear()
    {
        m_listeners_.clear();
    }
};
}