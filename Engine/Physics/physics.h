#pragma once
#include "bullet_debug_drawer.h"
#include "event_receivers.h"
#include "Components/component.h"

#include <bullet/btBulletDynamicsCommon.h>

namespace engine
{
class RigidbodyComponent;

/// <summary>
/// Bulletを使った物理シミュレーションを管理するクラスです。
/// </summary>
class Physics : public IFixedUpdateReceiver
{
    friend class Engine;
    friend class RigidbodyComponent;

    static std::shared_ptr<Physics> m_instance_;
    Vector3 m_gravity_;

    std::unique_ptr<btCollisionConfiguration> m_collision_configuration_;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher_;
    std::unique_ptr<btBroadphaseInterface> m_broadphase_;
    std::unique_ptr<btConstraintSolver> m_solver_;
    std::unique_ptr<btDynamicsWorld> m_world_;
    std::unique_ptr<BulletDebugDrawer> m_debug_drawer_;
    std::vector<std::weak_ptr<RigidbodyComponent>> m_rigidbodies_;

    using ContactPair = std::pair<const RigidbodyComponent *, const RigidbodyComponent *>;
    using CollisionPair = std::pair<Collision, Collision>;
    std::map<ContactPair, CollisionPair> m_current_contacts_;
    std::map<ContactPair, CollisionPair> m_previous_contacts_;

    using TriggerPair = std::pair<const std::shared_ptr<GameObject>, const std::shared_ptr<GameObject>>;
    std::set<TriggerPair> m_current_overlaps_;
    std::set<TriggerPair> m_previous_overlaps_;

    /// <summary>
    /// Physicsのインスタンスを生成し、UpdateManagerにFixedUpdateの登録を行います。既に初期化されている場合は例外を投げます。
    /// </summary>
    static void Init();

    /// <summary>
    /// BulletのWorldを作成し、重力とデバッグ表示を設定します。
    /// </summary>
    Physics();


    /// <summary>
    /// 衝突を始めた2つのGameObjectのOnCollisionEnterを呼び出します。
    /// </summary>
    static void OnCollisionStarted(const ContactPair &contact_pair, const CollisionPair &collision_pair);
    /// <summary>
    /// 衝突し続けている2つのGameObjectのOnCollisionStayを呼び出します。
    /// </summary>
    static void OnCollisionStayed(const ContactPair &contact_pair, const CollisionPair &collision_pair);
    /// <summary>
    /// 衝突が終わった2つのGameObjectのOnCollisionExitを呼び出します。
    /// </summary>
    static void OnCollisionExited(const ContactPair &contact_pair, const CollisionPair &collision_pair);

    /// <summary>
    /// targetのOnTriggerEnterを呼び出します。
    /// </summary>
    /// <param name="target">通知を受けるGameObject</param>
    /// <param name="other">重なった相手のGameObject</param>
    static void OnTriggerStarted(const std::shared_ptr<GameObject> &target, const std::shared_ptr<GameObject> &other);
    /// <summary>
    /// targetのOnTriggerStayを呼び出します。
    /// </summary>
    /// <param name="target">通知を受けるGameObject</param>
    /// <param name="other">重なっている相手のGameObject</param>
    static void OnTriggerStayed(const std::shared_ptr<GameObject> &target, const std::shared_ptr<GameObject> &other);
    /// <summary>
    /// targetのOnTriggerExitを呼び出します。
    /// </summary>
    /// <param name="target">通知を受けるGameObject</param>
    /// <param name="other">重なっていた相手のGameObject</param>
    static void OnTriggerExited(const std::shared_ptr<GameObject> &target, const std::shared_ptr<GameObject> &other);

    /// <summary>
    /// このフレームでTriggerと重なっているGameObjectの組を記録します。
    /// </summary>
    static void AddTriggerOverlap(const std::shared_ptr<GameObject> &a, const std::shared_ptr<GameObject> &b);

    /// <summary>
    /// RigidbodyとそのGhostObjectをBulletのWorldに追加します。
    /// </summary>
    /// <param name="rb">追加するRigidbody</param>
    static void AddRigidbody(const std::shared_ptr<RigidbodyComponent> &rb);
    /// <summary>
    /// RigidbodyとそのGhostObjectをBulletのWorldから取り除きます。
    /// </summary>
    /// <param name="rb">取り除くRigidbody</param>
    static void RemoveRigidbody(const std::shared_ptr<RigidbodyComponent> &rb);

    /// <summary>
    /// Bulletの接触情報を前回の結果と比較し、衝突の開始 / 継続 / 終了を通知します。
    /// </summary>
    void ProcessCollisions();
    /// <summary>
    /// Triggerとの重なりを前回の結果と比較し、重なりの開始 / 継続 / 終了を通知します。
    /// </summary>
    void ProcessTriggers();

public:
    int Order() override;
    /// <summary>
    /// 各Rigidbodyの状態をBulletに反映してシミュレーションを1ステップ進め、結果をRigidbodyに反映した後、衝突とTriggerの通知を行います。
    /// </summary>
    void OnFixedUpdate() override;

    /// <summary>
    /// BulletのWorldのデバッグ表示を描画します。
    /// </summary>
    static void DebugDraw();
    /// <summary>
    /// 重力を取得します。
    /// </summary>
    static Vector3 Gravity();
    /// <summary>
    /// 重力を設定します。
    /// </summary>
    /// <param name="gravity">重力</param>
    static void SetGravity(const Vector3 &gravity);

};
}