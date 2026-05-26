#pragma once

namespace engine
{
template <typename T>
class ObjectPool
{
    size_t m_max_size_ = 0;
    std::list<std::pair<T, bool>> m_objects_;
    std::function<T()> m_on_create_ = [] {
        return T();
    };

public:

    size_t MaxSize() const;
    void SetMaxSize(size_t max_size);

    T *Get();
    void Return(const T *object);
    void ReturnAll();

    explicit ObjectPool(size_t max_size, std::function<T()> on_create = [] {
        return T();
    });
};

template <typename T>
size_t ObjectPool<T>::MaxSize() const
{
    return m_max_size_;
}

template <typename T>
void ObjectPool<T>::SetMaxSize(const size_t max_size)
{
    for (size_t i = m_objects_.size(); i < max_size; ++i)
    {
        m_objects_.emplace_back(m_on_create_(), true);
    }
    m_max_size_ = max_size;
}

template <typename T>
T *ObjectPool<T>::Get()
{
    auto find_object = std::ranges::find_if(m_objects_, [](const auto &a) {
        return a.second;
    });
    if (find_object == m_objects_.end())
    {
        auto object = m_objects_.emplace_back(m_on_create_(), false);
        return &object.first;
    }

    find_object->second = false;
    return &find_object->first;
}

template <typename T>
void ObjectPool<T>::Return(const T *object)
{
    auto find_object = std::ranges::find_if(m_objects_, [&object](const auto &other) {
        return object == &other.first;
    });
    if (find_object == m_objects_.end())
        return;

    if (m_objects_.size() > m_max_size_)
    {
        m_objects_.erase(find_object);
        return;
    }

    find_object->second = true;
}

template <typename T>
void ObjectPool<T>::ReturnAll()
{
    for (auto &object : m_objects_)
    {
        object.second = true;
    }

    m_objects_.resize(m_max_size_);
}

template <typename T>
ObjectPool<T>::ObjectPool(size_t max_size, std::function<T()> on_create) :
    m_max_size_(max_size),
    m_on_create_(on_create)
{
    for (int i = 0; i < max_size; ++i)
    {
        m_objects_.emplace_back(m_on_create_(), true);
    }
}
}