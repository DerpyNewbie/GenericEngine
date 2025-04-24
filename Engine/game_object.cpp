#include "game_object.h"

#include "component.h"
#include "transform.h"
#include "scene.h"

namespace engine
{
GameObject::GameObject(): Object()
{}

void GameObject::SetName(const std::string &name)
{
    Object::SetName(name);
    RefreshPath();
}

std::shared_ptr<Transform> GameObject::Transform() const
{
    return GetComponent<engine::Transform>();
}

void GameObject::SetActive(const bool is_active)
{
    if (m_is_active_ == is_active)
    {
        return;
    }

    m_is_active_ = is_active;

    for (const auto &component : m_components_)
    {
        if (m_is_active_)
        {
            component->OnEnabled();
        }
        else
        {
            component->OnDisabled();
        }
    }
}

std::string GameObject::GetPath()
{
    return m_path_;
}

void GameObject::InvokeUpdate() const
{
    for (auto &component : m_components_)
    {
        if (!component->m_has_called_start_)
        {
            Logger::Log<GameObject>("[%s] Calling first start for %s",
                                    m_path_.c_str(),
                                    component->GetName().c_str());
            component->OnStart();
            component->m_has_called_start_ = true;
        }
        component->OnUpdate();
    }
}

void GameObject::InvokeFixedUpdate() const
{
    for (auto &component : m_components_)
    {
        if (!component->m_has_called_start_)
            continue;
        component->OnFixedUpdate();
    }
}

void GameObject::SetAsRootObject(const bool is_root_object)
{
    auto shared_this = shared_from_base<GameObject>();
    const auto scene = m_scene_.lock();
    const auto root_objects = &scene->m_root_game_objects_;
    const auto pos = std::ranges::find_if(
        *root_objects,
        [&shared_this](const std::shared_ptr<GameObject> &other) {
            return shared_this == other;
        });

    if (is_root_object && pos == root_objects->end())
    {
        // If we're root but HAVEN'T registered as root
        scene->m_root_game_objects_.push_back(shared_from_base<GameObject>());
    }
    else if (!is_root_object && pos != root_objects->end())
    {
        // If we're NOT root but has registered as root
        scene->m_root_game_objects_.erase(pos);
    }

    RefreshPath();
}

void GameObject::RefreshPath()
{
    const auto transform = Transform();
    const auto parent_transform = transform->Parent();

    m_path_.clear();
    m_path_ = parent_transform != nullptr
                  ? parent_transform->GameObject()->GetPath() + "/" + GetName()
                  : GetName();

    for (int i = 0; i < transform->ChildCount(); i++)
    {
        transform->GetChild(i)->GameObject()->RefreshPath();
    }
}
}