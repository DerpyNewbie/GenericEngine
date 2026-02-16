#pragma once

namespace engine
{
template <typename T>
class ObjectPool
{
    size_t m_max_size_ = 0;
    std::vector<std::pair<T, bool>> m_objects_;

public:
    std::function<T()> on_create = [] {
        return T();
    };

    size_t MaxSize() const;
    void SetMaxSize(size_t max_size);

    T *Get();
    void Return(const T &object);
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
    m_max_size_ = max_size;
}

template <typename T>
T *ObjectPool<T>::Get()
{
    auto find_object = std::ranges::find_if(m_objects_, [](const auto &a) {
        return a.second;
    });
    if (find_object == m_objects_.end())
        return nullptr;

    find_object->second = false;
    return &find_object->first;
}

template <typename T>
void ObjectPool<T>::Return(const T &object)
{
    auto object_ptr = &object;
    auto find_object = std::ranges::find_if(m_objects_, [&object_ptr](const auto &other) {
        return object_ptr == &other.first;
    });
    if (find_object == m_objects_.end())
        return;

    find_object->second = true;
}

template <typename T>
void ObjectPool<T>::ReturnAll()
{
    for (auto &object : m_objects_)
    {
        object.second = true;
    }
}
template <typename T>
ObjectPool<T>::ObjectPool(size_t max_size, std::function<T()> on_create) :
    m_max_size_(max_size),
    on_create(on_create)
{
    m_objects_.reserve(max_size);
    for (int i = 0; i < max_size; ++i)
    {
        m_objects_.emplace_back(on_create(), true);
    }
}
}