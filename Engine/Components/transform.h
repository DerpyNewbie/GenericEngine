#pragma once
#include "component.h"

namespace engine
{
/// <summary>
/// 位置、回転、Scaleと、親子関係を持つComponentです。
/// </summary>
class Transform : public Component
{
    bool m_is_dirty_ = true;
    Vector3 m_local_position_ = {};
    Quaternion m_local_rotation_ = Quaternion::Identity;
    Vector3 m_local_scale_ = {1, 1, 1};

    Matrix m_local_matrix_ = Matrix::Identity;
    Matrix m_world_matrix_ = Matrix::Identity;
    std::weak_ptr<Transform> m_parent_ = {};
    std::vector<std::shared_ptr<Transform>> m_children_;

    /// <summary>
    /// 位置、回転、Scaleを編集するGuiを表示します。
    /// </summary>
    /// <param name="is_local">Local空間の値を表示する場合 true、World空間の場合 false</param>
    void TransformGui(bool is_local);
    /// <summary>
    /// 移動、回転、拡大縮小からMatrixを作成します。
    /// </summary>
    static Matrix TRS(Vector3 translation, Quaternion rotation, Vector3 scale);
    /// <summary>
    /// LocalMatrixとWorldMatrixを計算し直します。
    /// </summary>
    void RecalculateMatrices();
    /// <summary>
    /// 自身とすべての子に、WorldMatrixの再計算が必要であることを記録します。
    /// </summary>
    void SetDirty();

public:
    /// <summary>
    /// Matrixを計算し直します。
    /// </summary>
    void OnValidate() override;
    /// <summary>
    /// GameObjectをSceneに登録し直し、Matrixを計算します。
    /// </summary>
    void OnAwake() override;
    /// <summary>
    /// すべての子のGameObjectを破棄し、親から自身を切り離します。
    /// </summary>
    void OnDestroy() override;
    void OnInspectorGui() override;

    /// <summary>
    /// 最後に計算されたLocalMatrixを取得します。
    /// </summary>
    [[nodiscard]] Matrix LocalMatrix() const;
    /// <summary>
    /// WorldMatrixを取得します。必要な場合は計算し直します。
    /// </summary>
    [[nodiscard]] Matrix WorldMatrix();
    /// <summary>
    /// 親のWorldMatrixを取得します。親がない場合は単位行列を返します。
    /// </summary>
    [[nodiscard]] Matrix ParentMatrix() const;
    /// <summary>
    /// Local空間からWorld空間に変換するMatrix(WorldMatrix)を取得します。
    /// </summary>
    [[nodiscard]] Matrix LocalToWorld();
    /// <summary>
    /// World空間からLocal空間に変換するMatrix(WorldMatrixの逆行列)を取得します。
    /// </summary>
    [[nodiscard]] Matrix WorldToLocal();

    /// <summary>
    /// World空間での位置を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Position();
    /// <summary>
    /// World空間での回転を取得します。
    /// </summary>
    [[nodiscard]] Quaternion Rotation();
    /// <summary>
    /// World空間でのScaleを取得します。
    /// </summary>
    [[nodiscard]] Vector3 Scale();

    /// <summary>
    /// 親に対する位置を取得します。
    /// </summary>
    [[nodiscard]] Vector3 LocalPosition() const;
    /// <summary>
    /// 親に対する回転を取得します。
    /// </summary>
    [[nodiscard]] Quaternion LocalRotation() const;
    /// <summary>
    /// 親に対するScaleを取得します。
    /// </summary>
    [[nodiscard]] Vector3 LocalScale() const;

    /// <summary>
    /// 親のTransformを取得します。
    /// </summary>
    /// <returns>親がない場合 nullptr</returns>
    [[nodiscard]] std::shared_ptr<Transform> Parent() const;
    /// <summary>
    /// 指定されたindexの子を取得します。範囲外の場合は例外を投げます。
    /// </summary>
    /// <param name="i">子のindex</param>
    [[nodiscard]] std::shared_ptr<Transform> GetChild(int i) const;
    /// <summary>
    /// 兄弟の中での自身の並び順を取得します。親がない場合は、SceneのRootの中での並び順を返します。
    /// </summary>
    [[nodiscard]] int GetSiblingIndex() const;

    /// <summary>
    /// 指定されたTransformの子であるかどうかを判定します。
    /// </summary>
    /// <param name="transform">親かどうかを調べるTransform</param>
    /// <param name="deep">true の場合は孫以降も含め、false の場合は直接の親のみを調べます。</param>
    [[nodiscard]] bool IsChildOf(const std::shared_ptr<Transform> &transform, bool deep = true) const;
    /// <summary>
    /// 子の数を取得します。
    /// </summary>
    [[nodiscard]] int ChildCount() const;

    /// <summary>
    /// 親を設定します。nullptrを渡すと親から切り離されます。
    /// </summary>
    /// <param name="next_parent">新しい親</param>
    void SetParent(const std::shared_ptr<Transform> &next_parent);
    /// <summary>
    /// 兄弟の中での自身の並び順を変更します。
    /// </summary>
    /// <param name="index">移動先のindex</param>
    void SetSiblingIndex(int index);
    /// <summary>
    /// 兄弟の中で先頭に移動します。
    /// </summary>
    void SetAsFirstSibling();
    /// <summary>
    /// 兄弟の中で末尾に移動します。
    /// </summary>
    void SetAsLastSibling();

    /// <summary>
    /// World空間での位置を設定します。
    /// </summary>
    /// <param name="position">World空間での位置</param>
    void SetPosition(const Vector3 &position);
    /// <summary>
    /// World空間での回転を設定します。
    /// </summary>
    /// <param name="rotation">World空間での回転</param>
    void SetRotation(const Quaternion &rotation);
    /// <summary>
    /// World空間での位置と回転をまとめて設定します。
    /// </summary>
    /// <param name="position">World空間での位置</param>
    /// <param name="rotation">World空間での回転</param>
    void SetPositionAndRotation(const Vector3 &position, const Quaternion &rotation);

    /// <summary>
    /// 親に対する位置を設定します。
    /// </summary>
    /// <param name="local_position">親に対する位置</param>
    void SetLocalPosition(const Vector3 &local_position);
    /// <summary>
    /// 親に対する回転を設定します。
    /// </summary>
    /// <param name="local_rotation">親に対する回転</param>
    void SetLocalRotation(const Quaternion &local_rotation);
    /// <summary>
    /// 親に対する位置と回転をまとめて設定します。
    /// </summary>
    /// <param name="local_position">親に対する位置</param>
    /// <param name="local_rotation">親に対する回転</param>
    void SetLocalPositionAndRotation(const Vector3 &local_position, const Quaternion &local_rotation);
    /// <summary>
    /// 親に対するScaleを設定します。
    /// </summary>
    /// <param name="local_scale">親に対するScale</param>
    void SetLocalScale(const Vector3 &local_scale);

    /// <summary>
    /// Matrixを分解して、親に対する位置、回転、Scaleを設定します。分解に失敗した場合は初期値に戻します。
    /// </summary>
    /// <param name="matrix">LocalMatrix</param>
    void SetLocalMatrix(const Matrix &matrix);

    /// <summary>
    /// World空間での前方向を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Forward()
    {
        const auto rot = Rotation();
        return Vector3::Transform(Vector3::Forward, rot);
    }

    /// <summary>
    /// World空間での後ろ方向を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Back()
    {
        const auto rot = Rotation();
        return Vector3::Transform(Vector3::Backward, rot);
    }

    /// <summary>
    /// World空間での右方向を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Right()
    {
        const auto rot = Rotation();
        return Vector3::Transform(Vector3::Right, rot);
    }

    /// <summary>
    /// World空間での左方向を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Left()
    {
        const auto rot = Rotation();
        return Vector3::Transform(Vector3::Left, rot);
    }

    /// <summary>
    /// World空間での上方向を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Up()
    {
        const auto rot = Rotation();
        return Vector3::Transform(Vector3::Up, rot);
    }

    /// <summary>
    /// World空間での下方向を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Down()
    {
        const auto rot = Rotation();
        return Vector3::Transform(Vector3::Down, rot);
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_local_position_),
            CEREAL_NVP(m_local_rotation_),
            CEREAL_NVP(m_local_scale_),
            CEREAL_NVP(m_parent_),
            CEREAL_NVP(m_children_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Transform, 1)