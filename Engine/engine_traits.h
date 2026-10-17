#pragma once


/// <summary>
/// Engineで使う型特性(type traits)をまとめた構造体です。
/// </summary>
struct engine_traits
{
    /// <summary>
    /// Tがstd::vectorかどうかを判定します。
    /// </summary>
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

    /// <summary>
    /// Tがstd::vectorの場合はその要素の型を、そうでない場合はT自身を返します。
    /// </summary>
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