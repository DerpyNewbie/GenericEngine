#pragma once

namespace engine
{
struct Rect
{
    Vector2 pos;
    Vector2 size;

    Rect(Vector2 pos, Vector2 size);

    bool Contains(Vector2 pos) const;
};
}