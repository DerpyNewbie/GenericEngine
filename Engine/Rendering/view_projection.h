#pragma once

namespace engine
{
/// <summary>
/// ViewとProjectionのMatrixをまとめた構造体です。
/// </summary>
struct ViewProjection
{
    /// <summary>
    /// ViewMatrixとProjectionMatrixを単位行列で初期化します。
    /// </summary>
    ViewProjection();
    Matrix matrices[2];
};
}