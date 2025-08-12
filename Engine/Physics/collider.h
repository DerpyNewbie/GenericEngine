#pragma once
#include "rigidbody_component.h"
#include "Components/component.h"

namespace engine
{
class Collider : public Component
{
    friend class RigidbodyComponent;
    friend class CompoundShape;

    std::weak_ptr<RigidbodyComponent> m_rigidbody_;
    Vector3 m_offset_ = {0, 0, 0};
    bool m_is_trigger_ = false;
    bool m_is_dirty_ = true;
    bool m_is_registered_ = false;

    virtual void UpdateShape() = 0;
    virtual btCollisionShape *GetShape() = 0;

    void AddToRigidbody();
    void RemoveFromRigidbody();
    void UpdateRigidbody();

protected:
    void MarkDirty();

public:
    void OnInspectorGui() override;
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    [[nodiscard]] std::shared_ptr<RigidbodyComponent> Rigidbody() const;
    [[nodiscard]] bool IsTrigger() const;
    [[nodiscard]] Vector3 Offset() const;

    void SetTrigger(bool trigger);
    void SetOffset(Vector3 offset);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), m_rigidbody_);
    }
};
}