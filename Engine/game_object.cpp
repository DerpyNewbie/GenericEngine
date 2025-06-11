#include "game_object.h"
#include "scene.h"
#include "scene_manager.h"

#include "Components/component.h"
#include "Components/transform.h"

#include "cereal/archives/json.hpp"

namespace engine
{
GameObject::GameObject(): Object()
{}
void GameObject::OnConstructed()
{
    Object::OnConstructed();
    AddComponent<engine::Transform>();
    SceneManager::MoveGameObject(shared_from_base<GameObject>(), SceneManager::GetActiveScene());
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
std::shared_ptr<Scene> GameObject::Scene() const
{
    return m_scene_.lock();
}

std::string GameObject::Path() const
{
    if (Transform() == nullptr || Transform()->Parent() == nullptr)
        return m_name_;
    return Transform()->Parent()->GameObject()->Path() + "/" + m_name_;
}

std::string GameObject::PathFrom(const std::shared_ptr<GameObject> &parent)
{
    if (Transform() == nullptr || parent->Transform() == nullptr || !Transform()->IsChildOf(parent->Transform()))
        return m_name_;
    const auto path = Path();
    const auto parent_path = parent->Path();

    return path.substr(parent_path.size());
}

void GameObject::InvokeUpdate() const
{
    for (auto &component : m_components_)
    {
        if (!component->m_has_called_start_)
        {
            Logger::Log<GameObject>("[%s] Calling first start for %s",
                                    Path().c_str(),
                                    component->Name().c_str());
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
}

template <class Archive>
void GameObject::serialize(Archive &ar)
{
    ar(cereal::base_class<Object>(this), CEREAL_NVP(m_scene_), CEREAL_NVP(m_is_active_), CEREAL_NVP(m_components_));
}
}

template void engine::GameObject::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive &);
template void engine::GameObject::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive &);

CEREAL_REGISTER_TYPE(engine::GameObject)