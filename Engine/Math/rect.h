#pragma once

namespace engine
{
/// <summary>
/// 位置とサイズを持つ矩形です。
/// </summary>
struct Rect
{
    Vector2 pos;
    Vector2 size;

    /// <summary>
    /// 位置と大きさを指定してRectを作成します。
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="size">大きさ</param>
    Rect(Vector2 pos, Vector2 size);

};
}