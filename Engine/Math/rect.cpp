#include "pch.h"
#include "rect.h"

engine::Rect::Rect(Vector2 pos, Vector2 size): pos(pos), size(size)
{
}
bool engine::Rect::Contains(const Vector2 pos) const
{
    const auto left_top = this->pos - this->size * 0.5f;
    const auto right_down = this->pos + this->size * 0.5f;
    return left_top.x <= pos.x && right_down.x >= pos.x && left_top.y <= pos.y && right_down.y >= pos.y;
}
