#include "pch.h"

#include "game_object.h"

#include "application.h"
#include "scene.h"
#include "scene_manager.h"

#include "Components/component.h"
#include "Components/transform.h"

namespace engine
{
GameObject::GameObject() :
    Object()
{ }
void GameObject::OnConstructed()
{
    Object::OnConstructed();
    SceneManager::MoveGameObject(shared_from_base<GameObject>(), SceneManager::GetActiveScene());
    if (Transform() == nullptr)
        AddComponent<engine::Transform>();

    UpdateActiveInHierarchy(!Application::IsPlayMode());
}

void GameObject::OnDeserialized()
{
    UpdateActiveInHierarchy(!Application::IsPlayMode());
}

void GameObject::OnDestroy()
{
    // notify the scene that there is a destroying game object
    const auto scene = Scene();
    if (scene != nullptr)
    {
        scene->MarkDestroyingGameObject();
    }

    SetActive(false);

    for (const auto &component : m_components_)
    {
        Destroy(component);
    }
}
std::shared_ptr<Transform> GameObject::Transform() const
{
    return GetComponent<engine::Transform>();
}

bool GameObject::IsActiveInHierarchy() const
{
    return m_is_active_in_hierarchy_;
}

bool GameObject::IsActiveSelf() const
{
    return m_is_active_self_;
}

void GameObject::SetActive(const bool is_active)
{
    if (m_is_active_self_ == is_active)
    {
        return;
    }

    m_is_active_self_ = is_active;
    UpdateActiveInHierarchy(true);
}

std::shared_ptr<Scene> GameObject::Scene() const
{
    return m_scene_.lock();
}

std::string GameObject::Path() const
{
    if (Transform() == nullptr || Transform()->Parent() == nullptr)
        return Name();
    return Transform()->Parent()->GameObject()->Path() + "/" + Name();
}

std::string GameObject::PathFrom(const std::shared_ptr<GameObject> &parent) const
{
    if (Transform() == nullptr || parent->Transform() == nullptr || !Transform()->IsChildOf(parent->Transform()))
        return Name();
    const auto path = Path();
    const auto parent_path = parent->Path();

    return path.substr(parent_path.size());
}

void GameObject::RemoveDestroyedComponents()
{
    std::erase_if(
        m_components_,
        [](const auto &component) {
            return component->IsDestroying();
        }
    );
}

void GameObject::InvokeOnUpdate()
{
    if (IsDestroying() || !IsActiveInHierarchy())
    {
        return;
    }

    RemoveDestroyedComponents();

    const auto transform = Transform();
    if (transform != nullptr)
    {
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child_game_object = transform->GetChild(i)->GameObject();
            child_game_object->InvokeOnUpdate();
        }
    }
}

void GameObject::InvokeOnFixedUpdate() const
{
    if (!IsActiveInHierarchy())
    {
        return;
    }

    for (auto &component : m_components_)
    {
        component->InvokeOnFixedUpdate();
    }

    const auto transform = Transform();
    if (transform != nullptr)
    {
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child_game_object = transform->GetChild(i)->GameObject();
            child_game_object->InvokeOnFixedUpdate();
        }
    }
}

void GameObject::InvokeOnValidate()
{
    RemoveDestroyedComponents();

    for (const auto &component : m_components_)
    {
        component->OnValidate();
    }
}

void GameObject::UpdateActiveInHierarchy(const bool invoke_component_events)
{
    const auto transform = Transform();
    const auto parent = transform != nullptr ? transform->Parent() : nullptr;
    const auto active_in_hierarchy = parent == nullptr ? m_is_active_self_ : parent->GameObject()->m_is_active_in_hierarchy_ && m_is_active_self_;

    if (m_is_active_in_hierarchy_ == active_in_hierarchy)
    {
        return;
    }

    m_is_active_in_hierarchy_ = active_in_hierarchy;

    if (invoke_component_events)
    {
        for (const auto &component : m_components_)
        {
            if (m_is_active_in_hierarchy_)
            {
                component->InvokeOnEnabled();
            }
            else
            {
                component->InvokeOnDisabled();
            }
        }

        if (!Application::IsPlayMode())
        {
            InvokeOnValidate();
        }
    }

    if (transform != nullptr)
    {
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            transform->GetChild(i)->GameObject()->UpdateActiveInHierarchy(invoke_component_events);
        }
    }
}

void GameObject::InvokeOnCollisionEnter(const Collision &collision) const
{
    if (IsActiveInHierarchy() == false)
    {
        return;
    }

    for (const auto &component : m_components_)
    {
        component->OnCollisionEnter(collision);
    }

    const auto transform = Transform();
    if (transform != nullptr)
    {
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child = transform->GetChild(i)->GameObject();
            child->InvokeOnCollisionEnter(collision);
        }
    }
}

void GameObject::InvokeOnCollisionStay(const Collision &collision) const
{
    if (IsActiveInHierarchy() == false)
    {
        return;
    }

    for (const auto &component : m_components_)
    {
        component->OnCollisionStay(collision);
    }

    const auto transform = Transform();
    if (transform != nullptr)
    {
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child = transform->GetChild(i)->GameObject();
            child->InvokeOnCollisionStay(collision);
        }
    }
}

void GameObject::InvokeOnCollisionExit(const Collision &collision) const
{
    if (IsActiveInHierarchy() == false)
    {
        return;
    }

    for (const auto &component : m_components_)
    {
        component->OnCollisionExit(collision);
    }

    const auto transform = Transform();
    if (transform != nullptr)
    {
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child = transform->GetChild(i)->GameObject();
            child->InvokeOnCollisionExit(collision);
        }
    }
}

void GameObject::InvokeOnTriggerEnter(const std::shared_ptr<GameObject> &other) const
{
    if (IsActiveInHierarchy() == false)
    {
        return;
    }

    for (const auto &component : m_components_)
    {
        component->OnTriggerEnter(other);
    }

    const auto transform = Transform();
    if (transform != nullptr)
    {
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child = transform->GetChild(i)->GameObject();
            child->InvokeOnTriggerEnter(other);
        }
    }
}

void GameObject::InvokeOnTriggerStay(const std::shared_ptr<GameObject> &other) const
{
    if (IsActiveInHierarchy() == false)
    {
        return;
    }

    for (const auto &component : m_components_)
    {
        component->OnTriggerStay(other);
    }

    const auto transform = Transform();
    if (transform != nullptr)
    {
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child = transform->GetChild(i)->GameObject();
            child->InvokeOnTriggerStay(other);
        }
    }
}

void GameObject::InvokeOnTriggerExit(const std::shared_ptr<GameObject> &other) const
{
    if (IsActiveInHierarchy() == false)
    {
        return;
    }

    for (const auto &component : m_components_)
    {
        component->OnTriggerExit(other);
    }

    const auto transform = Transform();
    if (transform != nullptr)
    {
        for (int i = 0; i < transform->ChildCount(); i++)
        {
            const auto child = transform->GetChild(i)->GameObject();
            child->InvokeOnTriggerExit(other);
        }
    }
}

template <class Archive>
void GameObject::serialize(Archive &ar)
{
    ar(cereal::base_class<Object>(this), CEREAL_NVP(m_is_active_self_), CEREAL_NVP(m_components_));

    if (m_scene_.expired())
    {
        SceneManager::MoveGameObject(shared_from_base<GameObject>(), SceneManager::GetActiveScene());
    }
}
}

template void engine::GameObject::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive &);
template void engine::GameObject::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive &);
template void engine::GameObject::serialize<cereal::XMLOutputArchive>(cereal::XMLOutputArchive &);
template void engine::GameObject::serialize<cereal::XMLInputArchive>(cereal::XMLInputArchive &);
template void engine::GameObject::serialize<cereal::PortableBinaryOutputArchive>(cereal::PortableBinaryOutputArchive &);
template void engine::GameObject::serialize<cereal::PortableBinaryInputArchive>(cereal::PortableBinaryInputArchive &);

CEREAL_REGISTER_TYPE(engine::GameObject)