#pragma once

class MemUtil
{
public:
    template <typename T>
    static T AlignUp(T size, size_t alignment);
};

template <typename T>
T MemUtil::AlignUp(T size, const size_t alignment)
{
    return static_cast<T>((size + (alignment - 1)) & ~(alignment - 1));
}