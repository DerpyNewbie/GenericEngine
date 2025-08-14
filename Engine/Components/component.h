#pragma once

namespace engine
{
class GameObject;

struct Collision
{
    GameObject *other;
    Vector3 average_normal;
};

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

    virtual void OnCollisionEnter(const Collision &collision)
    {}

    virtual void OnCollisionStay(const Collision &collision)
    {}

    virtual void OnCollisionExit(const Collision &collision)
    {}

    virtual void OnTriggerEnter(const std::shared_ptr<GameObject> &other)
    {}

    virtual void OnTriggerStay(const std::shared_ptr<GameObject> &other)
    {}

    virtual void OnTriggerExit(const std::shared_ptr<GameObject> &other)
    {}

    virtual void OnInspectorGui();

    [[nodiscard]] std::shared_ptr<GameObject> GameObject() const;

    template <class Archive>
    void serialize(Archive &ar);
};
}