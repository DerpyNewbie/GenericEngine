#pragma once
#include "Physics/collision.h"

namespace engine
{
class GameObject;

/// <summary>
/// A Component definition that can be attached to a GameObject for defining custom behavior of GameObjects.  
/// </summary>
/// <remarks>
/// Component events are called in this order:
/// 1. OnAwake - once in a lifetime
/// 2. OnEnabled - every time this component gets enabled
/// 3. OnStart - once in a lifetime
/// 4. OnUpdate - every frame
/// 5. OnFixedUpdate - every fixed timing
/// 6. OnDisabled - every time this component gets disabled
/// 7. OnDestroy - once in a lifetime, on GC cycle
/// </remarks>
class Component : public Object
{
    friend class GameObject;

    bool m_has_called_awake_ = false;
    bool m_has_called_start_ = false;
    bool m_has_called_enabled_ = false;
    bool m_has_called_disabled_ = false;
    std::weak_ptr<GameObject> m_game_object_ = {};

    void EnsurePrepared();
    void InvokeOnAwake();
    void InvokeOnEnabled();
    void InvokeOnStart();
    void InvokeOnDisabled();
    void InvokeOnUpdate();
    void InvokeOnFixedUpdate();

public:
    Component();

    /// <summary>
    /// Called only in Editor mode when the Object state has changed. 
    /// </summary>
    virtual void OnValidate()
    { }

    /// <summary>
    /// Called only once and the first frame of this component.
    /// </summary>
    /// <remarks>
    /// When a component gets added, This event will be called before its added component gets returned.  
    /// </remarks>
    virtual void OnAwake()
    { }

    /// <summary>
    /// Called only once and the first frame of this component.
    /// Called after <see cref="OnAwake"/> and <see cref="OnEnabled"/>.
    /// </summary>
    virtual void OnStart()
    { }

    /// <summary>
    /// Called when this component or parent GameObject has been enabled.
    /// </summary>
    virtual void OnEnabled()
    { }

    /// <summary>
    /// Called when this component or parent GameObject has been disabled.
    /// </summary>
    virtual void OnDisabled()
    { }

    /// <summary>
    /// Called every fixed time interval.
    /// </summary>
    virtual void OnFixedUpdate()
    { }

    /// <summary>
    /// Called every frame.
    /// </summary>
    virtual void OnUpdate()
    { }

    /// <summary>
    /// Called when attached GameObject is rendering a object.
    /// </summary>
    /// <remarks>
    /// TODO: might be unused?
    /// </remarks>
    virtual void OnRender()
    { }

    /// <summary>
    /// Called when Rigidbody has started colliding with other Rigidbody.
    /// </summary>
    /// <param name="collision"></param>
    virtual void OnCollisionEnter(const Collision &collision)
    { }

    /// <summary>
    /// Called when Rigidbody is still colliding with other Rigidbody.
    /// </summary>
    /// <param name="collision"></param>
    virtual void OnCollisionStay(const Collision &collision)
    { }

    /// <summary>
    /// Called when Rigidbody has stopped colliding with other Rigidbody.
    /// </summary>
    /// <param name="collision"></param>
    virtual void OnCollisionExit(const Collision &collision)
    { }

    /// <summary>
    /// Called when Rigidbody has entered Trigger Collider.
    /// </summary>
    /// <param name="other"></param>
    virtual void OnTriggerEnter(const std::shared_ptr<GameObject> &other)
    { }

    /// <summary>
    /// Called when Rigidbody has stayed in Trigger Collider.
    /// </summary>
    /// <param name="other"></param>
    virtual void OnTriggerStay(const std::shared_ptr<GameObject> &other)
    { }

    /// <summary>
    /// Called when Rigidbody has exited Trigger Collider. 
    /// </summary>
    /// <param name="other"></param>
    virtual void OnTriggerExit(const std::shared_ptr<GameObject> &other)
    { }

    /// <summary>
    /// Called only in Editor mode when the Component is being rendered in an Inspector window. 
    /// </summary>
    /// <remarks>
    /// Use ImGui functions to render editable properties of this component.
    /// </remarks>
    virtual void OnInspectorGui();

    [[nodiscard]] std::shared_ptr<GameObject> GameObject() const;

    template <class Archive>
    void serialize(Archive &ar, uint32_t version);
};
}

CEREAL_CLASS_VERSION(engine::Component, 1)