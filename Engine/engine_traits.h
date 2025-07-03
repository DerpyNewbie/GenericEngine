#pragma once

struct engine_traits
{
    template <typename T>
    static constexpr bool is_vector()
    {
        return std::is_same_v<std::remove_reference_t<T>, std::vector<
                                  typename T::value_type, typename T::allocator_type>> || std::is_same_v<
                   std::remove_reference_t<T>, std::vector<typename T::value_type>>;
    }

    template<typename T>
    struct vector_element_type;
    
    template <typename T, typename Alloc>
    struct vector_element_type<std::vector<T,Alloc>>{
        using type = T;
    };
};

