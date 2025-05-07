#pragma once
#include <vector>

#include <cereal/types/vector.hpp>

#include "logger.h"
#include "object.h"
#include "component.h"
#include "engine_util.h"
#include "transform.h"

namespace engine {
class Scene;

class GameObject final : public Object {
public:
  explicit GameObject();

  void OnConstructed() override;

  std::shared_ptr<Transform> Transform() const;

  bool IsActive() const { return m_is_active_; }

  void SetActive(bool is_active);
   
  std::shared_ptr<Scene> Scene() const;
    
  std::string Path() const;

  template <typename T>
  std::shared_ptr<T> AddComponent() {
    static_assert(std::is_base_of<Component, T>(),
                  "Base type is not Component.");

    auto instance = Object::Instantiate<T>(EngineUtil::GetTypeName(typeid(T).name()));
    auto converted_instance = std::dynamic_pointer_cast<Component>(instance);
    converted_instance->m_game_object_ = shared_from_base<GameObject>();
    m_components_.push_back(converted_instance);

    converted_instance->OnAwake();
    Logger::Log<GameObject>("(%s): Added component %s", Path().c_str(),  converted_instance->m_name_.c_str());
    return instance;
  }

  template <typename T>
  std::shared_ptr<T> GetComponent() const {
    static_assert(std::is_base_of<Component, T>(),
                  "Base type is not Component.");
    for (const auto &comp : m_components_) {
      auto instance = std::dynamic_pointer_cast<T>(comp);
      if (instance != nullptr) return instance;
    }

    return nullptr;
  }

  template <typename T>
  std::vector<std::shared_ptr<T>> GetComponents() const {
    static_assert(std::is_base_of<Component, T>(),
                  "Base type is not Component.");
    std::vector<std::shared_ptr<T>> results = {};
    for (const auto &comp : m_components_) {
      auto instance = std::dynamic_pointer_cast<T>(comp);
      if (instance != nullptr) results.push_back(instance);
    }

    return results;
  }

  template <typename T>
  std::shared_ptr<T> GetComponentInParent() {
    auto result = GetComponent<T>();
    if (result != nullptr) return result;
    const auto parent = Transform()->Parent();
    if (parent == nullptr) return nullptr;
    return parent->GameObject()->GetComponentInParent<T>();
  }

  template <typename T>
  std::vector<std::shared_ptr<T>> GetComponentsInParent() {
    auto result = GetComponents<T>();
    const auto parent = Transform()->Parent();
    if (parent == nullptr) return result;
    auto parent_result = parent->GameObject()->GetComponentsInParent<T>();
    result.insert(result.end(), parent_result.begin(), parent_result.end());
    return result;
  }

  template <typename T>
  std::vector<std::shared_ptr<T>> GetComponentInChildren() {
    auto result = GetComponent<T>();
    if (result != nullptr) return result;
    const auto transform = Transform();
    for (int i = 0; i < transform->ChildCount(); i++) {
      const auto child = transform->GetChild(i)->GameObject();
      result = child->GetComponentInChildren<T>();
      if (result != nullptr) return result;
    }

    return nullptr;
  }

  template <typename T>
  std::vector<std::shared_ptr<T>> GetComponentsInChildren() {
    std::vector<std::shared_ptr<T>> result = GetComponents<T>();
    const auto transform = Transform();
    for (int i = 0; i < transform->ChildCount(); i++) {
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

  bool m_is_active_ = true;
  std::weak_ptr<engine::Scene> m_scene_ = {};
  std::vector<std::shared_ptr<Component>> m_components_ = {};

  void InvokeUpdate() const;
  void InvokeFixedUpdate() const;

  void SetAsRootObject(bool is_root_object);

public:
  template <class Archive>
  void serialize(Archive& ar)
  {
    ar(CEREAL_NVP(m_scene_),CEREAL_NVP(m_name_), CEREAL_NVP(m_is_active_),CEREAL_NVP(m_components_));
  }
};
}