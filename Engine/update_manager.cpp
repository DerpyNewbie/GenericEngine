#include "pch.h"

#include "update_manager.h"

namespace engine
{
namespace
{
template <typename It, typename V>
void Subscribe(It &it, V &v)
{
    it.emplace_back(v);
    std::ranges::sort(it, [](const auto &a, const auto &b) {
        return a.lock()->Order() < b.lock()->Order();
    });
}

template <typename It, typename Func>
void Update(It &it, Func func)
{
    auto begin_it = it.begin();
    auto end_it = it.end();
    for (; begin_it != end_it; ++begin_it)
    {
        auto locked = (*begin_it).lock();
        if (locked == nullptr)
        {
            Logger::Warn<UpdateManager>("Expired pointer for %s detected!",
                                        Logger::GetTypeName(typeid(It).name()).c_str());
            continue;
        }

        func(locked);
    }
}

template <typename It, typename V>
void Erase(It &it, V &v)
{
    const auto pos = std::ranges::find_if(it, [&](const auto &r) {
        return r.lock() == v;
    });
    if (pos == it.end())
        return;
    it.erase(pos);
}
}

void UpdateManager::InvokeUpdate()
{
    m_in_update_cycle_ = true;
    Update(m_update_receivers_, [&](const auto &receiver) {
        receiver->OnUpdate();
    });
    m_in_update_cycle_ = false;
    PostFix();
}

void UpdateManager::InvokeFixedUpdate()
{
    m_in_fixed_update_cycle_ = true;
    Update(m_fixed_update_receivers_, [&](const auto &receiver) {
        receiver->OnFixedUpdate();
    });
    m_in_fixed_update_cycle_ = false;
    PostFix();
}

void UpdateManager::InvokeRender()
{
    m_in_render_cycle_ = true;
    Update(m_render_receivers_, [&](const auto &receiver) {
        receiver->Render();
    });
    m_in_render_cycle_ = false;
    PostFix();
}

void UpdateManager::InvokeGarbageCollect()
{
    m_in_garbage_collect_cycle_ = true;
    Update(m_garbage_collect_receivers_, [&](const auto &receiver) {
        receiver->OnGarbageCollect();
    });
    m_in_garbage_collect_cycle_ = false;
    PostFix();
}

void UpdateManager::PostFix()
{
    while (!m_in_cycle_buffer_.empty())
    {
        auto func = m_in_cycle_buffer_.front();
        func();
        m_in_cycle_buffer_.pop();
    }
}

void UpdateManager::SubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    if (InUpdateCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            SubscribeUpdate(receiver);
        });
        return;
    }

    Subscribe(m_update_receivers_, receiver);
}

void UpdateManager::UnsubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    if (InUpdateCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            UnsubscribeUpdate(receiver);
        });
        return;
    }

    Erase(m_update_receivers_, receiver);
}

void UpdateManager::SubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    if (InFixedUpdateCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            SubscribeFixedUpdate(receiver);
        });
        return;
    }

    Subscribe(m_fixed_update_receivers_, receiver);
}

void UpdateManager::UnsubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    if (InFixedUpdateCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            UnsubscribeFixedUpdate(receiver);
        });
        return;
    }

    Erase(m_fixed_update_receivers_, receiver);
}

void UpdateManager::SubscribeRender(const std::shared_ptr<IRenderReceiver> &receiver)
{
    if (InRenderCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            SubscribeRender(receiver);
        });
        return;
    }

    Subscribe(m_render_receivers_, receiver);
}

void UpdateManager::UnsubscribeRender(const std::shared_ptr<IRenderReceiver> &receiver)
{
    if (InRenderCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            UnsubscribeRender(receiver);
        });
        return;
    }

    Erase(m_render_receivers_, receiver);
}

void UpdateManager::SubscribeGarbageCollect(const std::shared_ptr<IGarbageCollectReceiver> &receiver)
{
    if (InGarbageCollectCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            SubscribeGarbageCollect(receiver);
        });
        return;
    }

    Subscribe(m_garbage_collect_receivers_, receiver);
}

void UpdateManager::UnsubscribeGarbageCollect(const std::shared_ptr<IGarbageCollectReceiver> &receiver)
{
    if (InGarbageCollectCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            UnsubscribeGarbageCollect(receiver);
        });
        return;
    }

    Erase(m_garbage_collect_receivers_, receiver);
}

bool UpdateManager::InUpdateCycle()
{
    return m_in_update_cycle_;
}
bool UpdateManager::InFixedUpdateCycle()
{
    return m_in_fixed_update_cycle_;
}
bool UpdateManager::InRenderCycle()
{
    return m_in_render_cycle_;
}
bool UpdateManager::InGarbageCollectCycle()
{
    return m_in_garbage_collect_cycle_;
}
}