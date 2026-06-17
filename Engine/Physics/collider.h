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
    bool m_is_registered_ = false;

    virtual void UpdateShape() = 0;
    virtual std::shared_ptr<btCollisionShape> GetShape() = 0;

    void AddToRigidbody();
    void RemoveFromRigidbody();

protected:
    void ApplyChanges();

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
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_rigidbody_),
            CEREAL_NVP(m_offset_),
            CEREAL_NVP(m_is_trigger_)
        );
        
        UpdateShape();
    }
};
}

CEREAL_CLASS_VERSION(engine::Collider, 1)