#pragma once
#include <memory>
#include "object.h"

namespace engine
{
class GameObject;

class Component : public Object
{
private:
    friend class GameObject;

    bool m_has_called_start_ = false;
    std::weak_ptr<GameObject> m_game_object_ = {};

public:
    Component();

    virtual void OnAwake()
    {}

    virtual void OnStart()
    {}

    virtual void OnEnabled()
    {}

    virtual void OnDisabled()
    {}

    virtual void OnFixedUpdate()
    {}

    virtual void OnUpdate()
    {}

    virtual void OnRender()
    {}

    virtual void OnInspectorGui()
    {}

    virtual void OnDestroy()
    {}

    [[nodiscard]] std::shared_ptr<GameObject> GameObject() const
    {
        return m_game_object_.lock();
    }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(m_game_object_);
    }
};
}