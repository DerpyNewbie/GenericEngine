#pragma once
#include "component.h"

#include <DxLib.h>
#include <memory>
#include <vector>

namespace engine
{
class Transform : public Component
{
    MATRIX m_matrix_ = MGetIdent();
    std::shared_ptr<Transform> m_parent_ = nullptr;
    std::vector<std::weak_ptr<Transform>> m_children_;

    [[nodiscard]] MATRIX ParentMatrix() const;
    static MATRIX Compose(const VECTOR &scale, const MATRIX &rotation, const VECTOR &translation);

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
    void SetLocalRotation(const MATRIX &rotation);
    void SetLocalScale(VECTOR local_scale);


    [[nodiscard]] VECTOR Forward() const
    {
        return VTransform(VGet(0, 0, 1), LocalToWorld());
    }

    [[nodiscard]] VECTOR Back() const
    {
        return VTransform(VGet(0, 0, -1), LocalToWorld());
    }

    [[nodiscard]] VECTOR Right() const
    {
        return VTransform(VGet(1, 0, 0), LocalToWorld());
    }

    [[nodiscard]] VECTOR Left() const
    {
        return VTransform(VGet(-1, 0, 0), LocalToWorld());
    }

    [[nodiscard]] VECTOR Up() const
    {
        return VTransform(VGet(0, 1, 0), LocalToWorld());
    }

    [[nodiscard]] VECTOR Down() const
    {
        return VTransform(VGet(0, -1, 0), LocalToWorld());
    }
};
}