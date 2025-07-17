#pragma once
#include "rigidbody_component.h"
#include "Components/component.h"

namespace engine
{
class Collider : public Component
{
    friend class RigidbodyComponent;

    std::weak_ptr<RigidbodyComponent> m_rigidbody_;

    virtual void UpdateShape() = 0;
    virtual btCollisionShape *GetShape() = 0;

public:
    void OnInspectorGui() override;
    void OnEnabled() override;
    void OnDisabled() override;

    std::shared_ptr<RigidbodyComponent> Rigidbody() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), m_rigidbody_);
    }
};
}