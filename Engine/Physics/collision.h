#pragma once
#include "contact_point.h"

class btPersistentManifold;

namespace engine
{
class GameObject;
/// <summary>
/// 衝突の情報(相手のGameObject、接触点、力など)です。
/// </summary>
class Collision
{
    friend class Physics;

    std::weak_ptr<GameObject> m_other_ = {};
    btPersistentManifold *m_manifold_ = nullptr;
    Vector3 m_impulse_ = Vector3::Zero;
    bool m_is_body_0_ = false;

    /// <summary>
    /// 接触点の法線をめり込みの深さで重み付けして平均した法線を計算します。
    /// </summary>
    /// <returns>接触点がない場合は 0 のVector</returns>
    static Vector3 CalculateNormalFromManifold(btPersistentManifold *manifold);

    /// <summary>
    /// 衝突の情報を作成し、各接触点の力積の合計を計算します。
    /// </summary>
    /// <param name="other">衝突相手のGameObject</param>
    /// <param name="manifold">Bulletの接触情報</param>
    /// <param name="is_body_0">自身がmanifoldのbody0側である場合 true</param>
    Collision(const std::weak_ptr<GameObject> &other, btPersistentManifold *manifold, bool is_body_0);

public:
    /// <summary>
    /// 衝突相手のGameObjectを取得します。
    /// </summary>
    [[nodiscard]] std::shared_ptr<GameObject> Other() const;
    /// <summary>
    /// 接触点の数を取得します。
    /// </summary>
    [[nodiscard]] size_t ContactCount() const;
    /// <summary>
    /// 指定されたindexの接触点を取得します。範囲外の場合は例外を投げます。
    /// </summary>
    /// <param name="index">接触点のindex</param>
    [[nodiscard]] ContactPoint GetContact(int index) const;
    /// <summary>
    /// すべての接触点を取得します。
    /// </summary>
    [[nodiscard]] std::vector<ContactPoint> GetContacts() const;
};
}