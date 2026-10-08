#pragma once

class btManifoldPoint;
class btPersistentManifold;

namespace engine
{
/// <summary>
/// 衝突時の1つの接触点の情報です。
/// </summary>
class ContactPoint
{
    friend class Collision;

    const btManifoldPoint *m_bt_manifold_point_;
    bool m_is_body_0_;

    /// <summary>
    /// Bulletの接触点から作成します。
    /// </summary>
    /// <param name="bt_manifold_point">Bulletの接触点</param>
    /// <param name="is_body_0">自身が接触情報のbody0側である場合 true</param>
    explicit ContactPoint(const btManifoldPoint &bt_manifold_point, const bool is_body_0)
    {
        m_bt_manifold_point_ = &bt_manifold_point;
        m_is_body_0_ = is_body_0;
    }

public:
    /// <summary>
    /// World空間での、自身の側の接触位置を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Point() const;
    /// <summary>
    /// World空間での接触面の法線を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Normal() const;
    /// <summary>
    /// 接触点での2つの物体の距離を取得します。負の値はめり込んでいることを表します。
    /// </summary>
    [[nodiscard]] float Separation() const;
};
}