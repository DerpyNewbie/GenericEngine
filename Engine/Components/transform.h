#pragma once
#include "component.h"

#include <memory>
#include <vector>

namespace engine
{
class Transform : public Component
{
    Matrix m_matrix_ = Matrix::Identity;
    std::weak_ptr<Transform> m_parent_ = {};
    std::vector<std::shared_ptr<Transform>> m_children_;

    void RenderLocalTransformGui();
    void RenderGlobalTransformGui();

public:
    void OnInspectorGui() override;

    [[nodiscard]] Matrix LocalMatrix() const;
    [[nodiscard]] Matrix WorldMatrix() const;
    [[nodiscard]] Matrix LocalToWorld() const;
    [[nodiscard]] Matrix WorldToLocal() const;

    [[nodiscard]] Vector3 Position() const;
    [[nodiscard]] Quaternion Rotation() const;
    [[nodiscard]] Vector3 Scale() const;

    [[nodiscard]] Vector3 LocalPosition() const;
    [[nodiscard]] Quaternion LocalRotation() const;
    [[nodiscard]] Vector3 LocalScale() const;

    [[nodiscard]] std::shared_ptr<Transform> Parent() const;
    [[nodiscard]] std::shared_ptr<Transform> GetChild(int i) const;
    [[nodiscard]] bool IsChildOf(const std::shared_ptr<Transform> &transform, bool deep = true) const;
    [[nodiscard]] int ChildCount() const;
    void SetParent(const std::weak_ptr<Transform> &next_parent);

    void SetPosition(const Vector3 &position);
    void SetRotation(const Quaternion &rotation);
    void SetPositionAndRotation(const Vector3 &position, const Quaternion &rotation);

    void SetLocalPosition(const Vector3 &local_position);
    void SetLocalRotation(const Quaternion &local_rotation);
    void SetLocalPositionAndRotation(const Vector3 &local_position, const Quaternion &local_rotation);
    void SetLocalScale(const Vector3 &local_scale);

    void SetLocalMatrix(const Matrix &matrix);
    void SetTRS(const Vector3 &scale, const Quaternion &rotation, const Vector3 &position);

    [[nodiscard]] Vector3 Forward() const
    {
        return Rotation() * Vector3{0, 0, 1};
    }

    [[nodiscard]] Vector3 Back() const
    {
        return Rotation() * Vector3{0, 0, -1};
    }

    [[nodiscard]] Vector3 Right() const
    {
        return Rotation() * Vector3{1, 0, 0};
    }

    [[nodiscard]] Vector3 Left() const
    {
        return Rotation() * Vector3{-1, 0, 0};
    }

    [[nodiscard]] Vector3 Up() const
    {
        return Rotation() * Vector3{0, 1, 0};
    }

    [[nodiscard]] Vector3 Down() const
    {
        return Rotation() * Vector3{0, -1, 0};
    }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_matrix_),
           CEREAL_NVP(m_parent_),
           CEREAL_NVP(m_children_));
    }
};
}