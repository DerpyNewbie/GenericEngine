#pragma once
#include <DxLib.h>

namespace engine
{
using Byte = unsigned char;

struct Color
{
    Byte r = 0;
    Byte g = 0;
    Byte b = 0;
    Byte a = 255;

    constexpr Color() = default;
    
    constexpr explicit Color(const unsigned int hex_rgba)
    {
        r = (hex_rgba >> 24) & 0xFF;
        g = (hex_rgba >> 16) & 0xFF;
        b = (hex_rgba >> 8) & 0xFF;
        a = hex_rgba & 0xFF;
    }

    constexpr Color(const Byte r, const Byte g, const Byte b, const Byte a) : r(r), g(g), b(b), a(a)
    {}

    constexpr Color(const Byte r, const Byte g, const Byte b) :
        Color(static_cast<Byte>(r), static_cast<Byte>(g), static_cast<Byte>(b), static_cast<Byte>(0xFF))
    {};

    constexpr Color(const int r, const int g, const int b, const int a) :
        Color(static_cast<Byte>(r), static_cast<Byte>(g), static_cast<Byte>(b), static_cast<Byte>(a))
    {}

    constexpr Color(const int r, const int g, const int b) :
        Color(static_cast<Byte>(r), static_cast<Byte>(g), static_cast<Byte>(b))
    {}

    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr Color(const COLOR_U8 color) : Color(color.r, color.g, color.b, color.a)
    {}

    [[nodiscard]] unsigned int Rgba() const;

    [[nodiscard]] unsigned int Argb() const;

    void ToHsv(short &h, Byte &s, Byte &v) const;

    Color operator+(Color c) const;

    Color operator-(Color c) const;

    static Color FromHsv(short h, Byte s, Byte v);

    static Color FromDxLibColor(unsigned int hex_argb);
};
}