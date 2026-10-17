#pragma once

namespace engine
{
/// <summary>
/// Translation、Rotation、Scaleをまとめた構造体です。
/// </summary>
struct TRS
{
    Vector3 translation;
    Vector3 scale;
    Quaternion rotation;

    TRS() = default;
    /// <summary>
    /// Matrixを移動、回転、拡大縮小に分解してTRSを作成します。
    /// </summary>
    /// <param name="matrix">分解するMatrix</param>
    explicit TRS(Matrix matrix);

    /// <summary>
    /// 拡大縮小、回転、移動の順に合成したMatrixを取得します。
    /// </summary>
    [[nodiscard]] Matrix GetMatrix() const;

    /// <summary>
    /// 2つのTRSを補間します。translationとscaleはfromからtoへの線形補間、rotationは単位Quaternionからtoへの球面線形補間になります。
    /// </summary>
    /// <param name="t">補間の割合</param>
    static TRS Blend(const TRS &from, const TRS &to, float t);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(translation), CEREAL_NVP(scale), CEREAL_NVP(rotation));
    }
};
}