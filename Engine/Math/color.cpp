#include "color.h"

#include <algorithm>

namespace engine
{
unsigned int Color::Rgba() const
{
    return (r << 24) + (g << 16) + (b << 8) + a;
}

unsigned int Color::Argb() const
{
    return (a << 24) + (r << 16) + (g << 8) + b;
}

void Color::ToHsv(short &h, Byte &s, Byte &v) const
{
    const Byte max = std::max(std::max(r, g), b);
    const Byte min = std::min(std::min(r, g), b);
    if (r == g && g == b)
    {
        h = 0;
    }
    else if (r > g && r > b)
    {
        h = 60 * ((g - b) / (max - min));
    }
    else if (g > r && g > b)
    {
        h = 60 * ((b - r) / (max - min)) + 120;
    }
    else
    {
        h = 60 * ((r - g) / (max - min)) + 240;
    }

    if (h < 0)
    {
        h += 360;
    }

    s = (max - min) / max;
    v = max;
}

Color Color::FromHsv(const short h, const Byte s, const Byte v)
{
    const int max = v, min = max - ((s / 255) * max);
    if (h < 60)
    {
        return Color{max, (h / 60) * (max - min) + min, min};
    }

    if (h < 120)
    {
        return Color{((120 - h) / 60) * (max - min) + min, max, min};
    }

    if (h < 180)
    {
        return Color{min, max, ((h - 120) / 60) * (max - min) + min};
    }

    if (h < 240)
    {
        return Color{min, ((240 - h) / 60) * (max - min) + min, max};
    }

    if (h < 300)
    {
        return Color{((h - 240) / 60) * (max - min) + min, min, max};
    }

    if (h < 360)
    {
        return Color{max, min, ((360 - h) / 60) * (max - min) + min};
    }

    return Color{0xFF000000};
}

Color Color::FromDxLibColor(const unsigned int hex_argb)
{
    const unsigned int rgb = hex_argb & 0x00FFFFFF;
    const unsigned int a = hex_argb & 0xFF000000;
    return Color{rgb + a};
}

Color Color::operator+(const Color c) const
{
    return {r + c.r, g + c.g, b + c.b, a + c.a};
}

Color Color::operator-(const Color c) const
{
    return {r - c.r, g - c.g, b - c.b, a - c.a};
}
}