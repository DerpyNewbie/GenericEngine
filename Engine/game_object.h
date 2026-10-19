#pragma once

#include "application.h"
#include "engine_util.h"
#include "Components/component.h"
#include "Components/transform.h"

namespace engine
{
class Scene;

/// <summary>
/// Scene上に配置されるObjectです。Componentを持ち、Transformによる親子関係を持ちます。
/// </summary>
class GameObject final : public Object
{
public:
    /// <summary>
    /// 空のGameObjectを生成します。Sceneへの登録やTransformの追加はOnConstructedで行われます。
    /// </summary>
    explicit GameObject();

    /// <summary>
    /// ActiveなSceneに自身を登録し、Transformを持っていなければ追加します。
    /// </summary>
    void OnConstructed() override;

    /// <summary>
    /// Hierarchy上でのActive状態を更新します。
    /// </summary>
    void OnDeserialized() override;

    /// <summary>
    /// Sceneに破棄されることを通知し、自身を非Activeにした上で、すべてのComponentを破棄します。
    /// </summary>
    void OnDestroy() override;

    /// <summary>
    /// このGameObjectのTransformを取得します。
    /// </summary>
    [[nodiscard]] std::shared_ptr<Transform> Transform() const;

    /// <summary>
    /// 自身とすべての親がActiveであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsActiveInHierarchy() const;

    /// <summary>
    /// 自身に設定されているActive状態を取得します。親の状態は考慮されません。
    /// </summary>
    [[nodiscard]] bool IsActiveSelf() const;

    /// <summary>
    /// 自身のActive状態を設定し、自身と子のHierarchy上でのActive状態を更新します。
    /// </summary>
    /// <param name="is_active">Activeにする場合 true</param>
    void SetActive(bool is_active);

    /// <summary>
    /// このGameObjectが属しているSceneを取得します。
    /// </summary>
    [[nodiscard]] std::shared_ptr<Scene> Scene() const;

    /// <summary>
    /// Rootからこのオブジェクトまでの名前を"/"で繋いだPathを取得します。
    /// </summary>
    [[nodiscard]] std::string Path() const;

    /// <summary>
    /// 自身のPathから、指定された親のPathの部分を取り除いたPathを取得します。parentの子でない場合は自身の名前を返します。
    /// </summary>
    /// <param name="parent">基準となる親のGameObject</param>
    [[nodiscard]] std::string PathFrom(const std::shared_ptr<GameObject> &parent) const;

    /// <summary>
    /// T のComponentを生成し、このGameObjectに追加します。PlayMode中はOnAwake、Activeな場合はOnEnabledが呼ばれます。
    /// </summary>
    /// <returns>追加されたComponent</returns>
    template <typename T>
    std::shared_ptr<T> AddComponent()
    {
        static_assert(
            std::is_base_of<Component, T>(),
            "Base type is not Component."
        );

        std::shared_ptr<T> instance = Object::Instantiate<T>(EngineUtil::GetTypeName(typeid(T).name()));
        // const std::shared_ptr<Component> converted_instance = std::dynamic_pointer_cast<Component>(instance);
        instance->m_game_object_ = shared_from_base<GameObject>();
        m_components_.push_back(instance);

        if (Application::IsPlayMode())
        {
            instance->InvokeOnAwake();
        }

        if (IsActiveInHierarchy())
        {
            instance->InvokeOnEnabled();
        }

        Logger::Log<GameObject>("[%s]: Added component '%s'", Path().c_str(), instance->Name().c_str());
        return instance;
    }

    /// <summary>
    /// このGameObjectが持つ T のComponentを取得します。破棄待ちのものは除きます。
    /// </summary>
    /// <returns>見つからない場合 nullptr</returns>
    template <typename T>
    [[nodiscard]] std::shared_ptr<T> GetComponent() const
    {
        static_assert(
            std::is_base_of<Component, T>(),
            "Base type is not Component."
        );

        for (const auto &comp : m_components_)
        {
            auto instance = std::dynamic_pointer_cast<T>(comp);
            if (instance != nullptr && !instance->IsDestroying())
                return instance;
        }

        return nullptr;
    }

    /// <summary>
    /// このGameObjectが持つ T のComponentをすべて取得します。破棄待ちのものは除きます。
    /// </summary>
    template <typename T>
    [[nodiscard]] std::vector<std::shared_ptr<T>> GetComponents() const
    {
        static_assert(
            std::is_base_of<Component, T>(),
            "Base type is not Component."
        );

        std::vector<std::shared_ptr<T>> results = {};
        for (const auto &comp : m_components_)
        {
            auto instance = std::dynamic_pointer_cast<T>(comp);
            if (instance != nullptr && !instance->IsDestroying())
                results.push_back(instance);
        }

        return results;
    }

    /// <summary>
    /// このGameObjectが持つComponentをすべて取得します。破棄待ちのものは除きます。
    /// </summary>
    [[nodiscard]] std::vector<std::shared_ptr<Component>> GetComponents() const
    {
        auto filtered = m_components_ | std::ranges::views::filter(
                            [](const auto &comp) {
                                return !comp->IsDestroying();
                            }
                        );

        return std::vector(filtered.begin(), filtered.end());
    }

    /// <summary>
    /// 自身から親に向かって T のComponentを探し、最初に見つかったものを取得します。
    /// </summary>
    /// <returns>見つからない場合 nullptr</returns>
    template <typename T>
    [[nodiscard]] std::shared_ptr<T> GetComponentInParent()
    {
        auto result = GetComponent<T>();
        if (result != nullptr)
            return result;
        const auto parent = Transform()->Parent();
        if (parent == nullptr)
            return nullptr;
        return parent->GameObject()->GetComponentInParent<T>();
    }

    /// <summary>
    /// 自身とすべての親が持つ T のComponentをすべて取得します。
    /// </summary>
    template <typename T>
    [[nodiscard]] std::vector<std::shared_ptr<T>> GetComponentsInParent()
    {
        auto result = GetComponents<T>();
        const auto parent = Transform()->Parent();
        if (parent == nullptr)
            return result;
        auto parent_result = parent->GameObject()->GetComponentsInParent<T>();
        result.insert(result.end(), parent_result.begin(), parent_result.end());
        return result;
    }

    /// <summary>
    /// 自身から子に向かって T のComponentを探し、最初に見つかったものを取得します。
    /// </summary>
    /// <returns>見つからない場合 nullptr</returns>
    template <typename T>
    [[nodiscard]] std::shared_ptr<T> GetComponentInChildren()
    {
        auto result = GetComponent<T>();
        if (result != nullptr)
            return result;
        const auto transform = Transform();
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child = transform->GetChild(i)->GameObject();
            result = child->GetComponentInChildren<T>();
            if (result != nullptr)
                return result;
        }

        return nullptr;
    }

    /// <summary>
    /// 自身とすべての子が持つ T のComponentをすべて取得します。
    /// </summary>
    template <typename T>
    [[nodiscard]] std::vector<std::shared_ptr<T>> GetComponentsInChildren()
    {
        std::vector<std::shared_ptr<T>> result = GetComponents<T>();
        const auto transform = Transform();
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child = transform->GetChild(i)->GameObject();
            auto child_result = child->GetComponentsInChildren<T>();
            result.insert(result.end(), child_result.begin(), child_result.end());
        }

        return result;
    }

private:
    friend class Scene;
    friend class SceneManager;
    friend class Transform;
    friend class Physics;

    bool m_is_active_self_ = true;
    mutable bool m_is_active_in_hierarchy_ = false;
    std::weak_ptr<engine::Scene> m_scene_ = {};
    std::vector<std::shared_ptr<Component>> m_components_ = {};

    /// <summary>
    /// 破棄待ちのComponentをリストから取り除きます。
    /// </summary>
    void RemoveDestroyedComponents();

    /// <summary>
    /// 自身のComponentのOnUpdateを呼び出し、続けて子のGameObjectにも同じ処理を行います。非Activeの場合は何もしません。
    /// </summary>
    void InvokeOnUpdate();
    /// <summary>
    /// 自身のComponentのOnFixedUpdateを呼び出し、続けて子のGameObjectにも同じ処理を行います。非Activeの場合は何もしません。
    /// </summary>
    void InvokeOnFixedUpdate() const;
    /// <summary>
    /// 自身のComponentのOnValidateを呼び出します。
    /// </summary>
    void InvokeOnValidate();
    /// <summary>
    /// 親の状態と自身のActive状態からHierarchy上でのActive状態を更新し、子にも同じ処理を行います。
    /// </summary>
    /// <param name="invoke_component_events">状態が変わった時にComponentのOnEnabled / OnDisabledを呼び出すかどうか</param>
    void UpdateActiveInHierarchy(bool invoke_component_events);

    /// <summary>
    /// 自身と子のComponentのOnCollisionEnterを呼び出します。非Activeの場合は何もしません。
    /// </summary>
    /// <param name="collision">衝突の情報</param>
    void InvokeOnCollisionEnter(const Collision &collision) const;
    /// <summary>
    /// 自身と子のComponentのOnCollisionStayを呼び出します。非Activeの場合は何もしません。
    /// </summary>
    /// <param name="collision">衝突の情報</param>
    void InvokeOnCollisionStay(const Collision &collision) const;
    /// <summary>
    /// 自身と子のComponentのOnCollisionExitを呼び出します。非Activeの場合は何もしません。
    /// </summary>
    /// <param name="collision">衝突の情報</param>
    void InvokeOnCollisionExit(const Collision &collision) const;

    /// <summary>
    /// 自身と子のComponentのOnTriggerEnterを呼び出します。非Activeの場合は何もしません。
    /// </summary>
    /// <param name="other">接触した相手のGameObject</param>
    void InvokeOnTriggerEnter(const std::shared_ptr<GameObject> &other) const;
    /// <summary>
    /// 自身と子のComponentのOnTriggerStayを呼び出します。非Activeの場合は何もしません。
    /// </summary>
    /// <param name="other">接触した相手のGameObject</param>
    void InvokeOnTriggerStay(const std::shared_ptr<GameObject> &other) const;
    /// <summary>
    /// 自身と子のComponentのOnTriggerExitを呼び出します。非Activeの場合は何もしません。
    /// </summary>
    /// <param name="other">接触した相手のGameObject</param>
    void InvokeOnTriggerExit(const std::shared_ptr<GameObject> &other) const;

public:
    template <class Archive>
    void serialize(Archive &ar);
};
}