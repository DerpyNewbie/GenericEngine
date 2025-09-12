#pragma once

namespace engine
{
template <class... ArgTypes>
class Event
{
    std::list<std::function<void(ArgTypes...)>> m_listeners_;

public:
    void AddListener(std::function<void(ArgTypes...)> callback)
    {
        m_listeners_.push_back(callback);
    }

    void RemoveListener(std::function<void(ArgTypes...)> callback)
    {
        m_listeners_.remove(callback);
    }

    std::list<std::function<void(ArgTypes...)>> Listeners()
    {
        return m_listeners_;
    }

    void Invoke(ArgTypes... args)
    {
        for (auto &callback : m_listeners_)
            callback(args...);
    }

    void Clear()
    {
        m_listeners_.clear();
    }
};
}