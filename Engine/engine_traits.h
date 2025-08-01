#pragma once


struct engine_traits
{
    template <typename T>
    struct is_vector
    {
        static constexpr bool value = false;
    };

    template <typename T>
    struct is_vector<std::vector<T>>
    {
        static constexpr bool value = true;
    };

    template <typename T>
    struct vector_element_type
    {
        using type = T;
    };

    template <typename T>
    struct vector_element_type<std::vector<T>>
    {
        using type = T;
    };
};