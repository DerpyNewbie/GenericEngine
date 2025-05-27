#pragma once
#include "component.h"

#include <DxLib.h>
#include <memory>
#include <vector>

#include "dxlib_math.h"

namespace engine
{
class Transform : public Component
{
    MATRIX m_matrix_ = MGetIdent();
    std::weak_ptr<Transform> m_parent_ = {};
    std::vector<std::weak_ptr<Transform>> m_children_;

    [[nodiscard]] MATRIX ParentMatrix() const;

public:
    [[nodiscard]] MATRIX LocalToWorld() const;
    [[nodiscard]] MATRIX WorldToLocal() const;

    [[nodiscard]] VECTOR Position() const;
    [[nodiscard]] MATRIX Rotation() const;
    [[nodiscard]] VECTOR Scale() const;

    [[nodiscard]] VECTOR LocalPosition() const;
    [[nodiscard]] MATRIX LocalRotation() const;
    [[nodiscard]] VECTOR LocalScale() const;

    [[nodiscard]] std::shared_ptr<Transform> Parent() const;
    [[nodiscard]] std::shared_ptr<Transform> GetChild(int i) const;
    [[nodiscard]] bool IsChildOf(const std::shared_ptr<Transform> &transform, bool deep = true) const;
    [[nodiscard]] int ChildCount() const;
    void SetParent(const std::shared_ptr<Transform> &next_parent);

    void SetPosition(VECTOR position);
    void SetRotation(const MATRIX &rotation);

    void SetLocalPosition(VECTOR local_position);
    void SetLocalRotation(const MATRIX &local_rotation);
    void SetLocalScale(VECTOR local_scale);

    void SetLocalMatrix(const MATRIX &matrix);

    [[nodiscard]] VECTOR Forward() const
    {
        return VECTOR{0, 0, 1} * Rotation();
    }

    [[nodiscard]] VECTOR Back() const
    {
        return VECTOR{0, 0, -1} * Rotation();
    }

    [[nodiscard]] VECTOR Right() const
    {
        return VECTOR{1, 0, 0} * Rotation();
    }

    [[nodiscard]] VECTOR Left() const
    {
        return VECTOR{-1, 0, 0} * Rotation();
    }

    [[nodiscard]] VECTOR Up() const
    {
        return VECTOR{0, 1, 0} * Rotation();
    }

    [[nodiscard]] VECTOR Down() const
    {
        return VECTOR{0, -1, 0} * Rotation();
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