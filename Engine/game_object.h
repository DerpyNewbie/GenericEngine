#pragma once

#include "engine_util.h"
#include "Components/component.h"
#include "Components/transform.h"

namespace engine
{
class Scene;

class GameObject final : public Object
{
public:
    explicit GameObject();

    void OnConstructed() override;

    void OnDestroy() override;

    std::shared_ptr<Transform> Transform() const;

    bool IsActiveInHierarchy() const;

    bool IsActiveSelf() const;

    void SetActive(bool is_active);

    std::shared_ptr<Scene> Scene() const;

    std::string Path() const;

    std::string PathFrom(const std::shared_ptr<GameObject> &parent) const;

    template <typename T>
    std::shared_ptr<T> AddComponent()
    {
        static_assert(std::is_base_of<Component, T>(),
                      "Base type is not Component.");

        std::shared_ptr<T> instance = Object::Instantiate<T>(EngineUtil::GetTypeName(typeid(T).name()));
        // const std::shared_ptr<Component> converted_instance = std::dynamic_pointer_cast<Component>(instance);
        instance->m_game_object_ = shared_from_base<GameObject>();
        m_components_.push_back(instance);

        instance->OnAwake();
        if (IsActiveInHierarchy())
        {
            instance->OnEnabled();
        }
        Logger::Log<GameObject>("[%s]: Added component '%s'", Path().c_str(), instance->Name().c_str());
        return instance;
    }

    template <typename T>
    std::shared_ptr<T> GetComponent() const
    {
        static_assert(std::is_base_of<Component, T>(),
                      "Base type is not Component.");
        for (const auto &comp : m_components_)
        {
            auto instance = std::dynamic_pointer_cast<T>(comp);
            if (instance != nullptr)
                return instance;
        }

        return nullptr;
    }

    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponents() const
    {
        static_assert(std::is_base_of<Component, T>(),
                      "Base type is not Component.");
        std::vector<std::shared_ptr<T>> results = {};
        for (const auto &comp : m_components_)
        {
            auto instance = std::dynamic_pointer_cast<T>(comp);
            if (instance != nullptr)
                results.push_back(instance);
        }

        return results;
    }

    std::vector<std::shared_ptr<Component>> GetComponents() const
    {
        return m_components_;
    }

    template <typename T>
    std::shared_ptr<T> GetComponentInParent()
    {
        auto result = GetComponent<T>();
        if (result != nullptr)
            return result;
        const auto parent = Transform()->Parent();
        if (parent == nullptr)
            return nullptr;
        return parent->GameObject()->GetComponentInParent<T>();
    }

    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponentsInParent()
    {
        auto result = GetComponents<T>();
        const auto parent = Transform()->Parent();
        if (parent == nullptr)
            return result;
        auto parent_result = parent->GameObject()->GetComponentsInParent<T>();
        result.insert(result.end(), parent_result.begin(), parent_result.end());
        return result;
    }

    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponentInChildren()
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

    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponentsInChildren()
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

    bool m_is_active_self_ = true;
    std::weak_ptr<engine::Scene> m_scene_ = {};
    std::vector<std::shared_ptr<Component>> m_components_ = {};

    void InvokeUpdate();
    void InvokeFixedUpdate() const;
    void NotifyIsActiveChanged() const;

    void SetAsRootObject(bool is_root_object);

public:
    template <class Archive>
    void serialize(Archive &ar);
};
}