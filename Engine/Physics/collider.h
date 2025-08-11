#pragma once
#include "rigidbody_component.h"
#include "Components/component.h"

namespace engine
{
class Collider : public Component
{
    friend class RigidbodyComponent;

    std::weak_ptr<RigidbodyComponent> m_rigidbody_;
    bool m_is_dirty_ = true;

    virtual void UpdateShape() = 0;
    virtual btCollisionShape *GetShape() = 0;

    void AddToRigidbody();
    void RemoveFromRigidbody();

protected:
    void MarkDirty();

public:
    void OnInspectorGui() override;
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    std::shared_ptr<RigidbodyComponent> Rigidbody() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), m_rigidbody_);
    }
};
}