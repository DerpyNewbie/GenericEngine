#pragma once
#include <cstdint>
#include <optional>
#include <vector>

#include "Rendering/CaboRenderingSystem/RHI/handles.h"

namespace CRS
{
/// <summary>
/// 世代番号付きのスロット配列です。ハンドルで要素を引き、破棄済みのハンドルはnullptrになります。
/// </summary>
template <typename Tag, typename T>
class SlotPool
{
    struct Slot
    {
        std::optional<T> value;
        uint32_t generation = 1;
    };

    std::vector<Slot> m_slots_;
    std::vector<uint32_t> m_free_indices_;
    size_t m_count_ = 0;

public:
    using HandleType = Handle<Tag>;

    HandleType Add(T value)
    {
        uint32_t index;
        if (!m_free_indices_.empty())
        {
            index = m_free_indices_.back();
            m_free_indices_.pop_back();
        }
        else
        {
            index = static_cast<uint32_t>(m_slots_.size());
            m_slots_.emplace_back();
        }

        m_slots_[index].value.emplace(std::move(value));
        ++m_count_;
        return {index, m_slots_[index].generation};
    }

    /// <summary>
    /// 要素を取り除きます。世代番号を進めるので、以降は同じハンドルで引けなくなります。
    /// </summary>
    bool Remove(const HandleType handle)
    {
        if (Get(handle) == nullptr)
            return false;

        auto &slot = m_slots_[handle.index];
        slot.value.reset();
        ++slot.generation;
        m_free_indices_.push_back(handle.index);
        --m_count_;
        return true;
    }

    T *Get(const HandleType handle)
    {
        if (handle.index >= m_slots_.size())
            return nullptr;

        auto &slot = m_slots_[handle.index];
        if (slot.generation != handle.generation || !slot.value.has_value())
            return nullptr;

        return &*slot.value;
    }

    const T *Get(const HandleType handle) const
    {
        return const_cast<SlotPool *>(this)->Get(handle);
    }

    size_t Count() const
    {
        return m_count_;
    }

    /// <summary>
    /// 生きている要素すべてに対して関数を呼びます。
    /// </summary>
    template <typename Func>
    void ForEach(Func &&func)
    {
        for (uint32_t i = 0; i < m_slots_.size(); ++i)
        {
            if (m_slots_[i].value.has_value())
                func(HandleType{i, m_slots_[i].generation}, *m_slots_[i].value);
        }
    }

    template <typename Func>
    void ForEach(Func &&func) const
    {
        for (uint32_t i = 0; i < m_slots_.size(); ++i)
        {
            if (m_slots_[i].value.has_value())
                func(HandleType{i, m_slots_[i].generation}, *m_slots_[i].value);
        }
    }
};
}