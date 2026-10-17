#pragma once

#include "mesh_renderer.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class Transform;

/// <summary>
/// Boneによって変形するMeshを描画するRendererです。
/// </summary>
class SkinnedMeshRenderer : public MeshRenderer
{
    static bool m_draw_bones_;
    
    std::array<std::shared_ptr<StructuredBuffer>, RenderEngine::kFrame_Buffer_Count> m_bone_matrix_buffers_;
    std::array<std::shared_ptr<DescriptorHandle>, RenderEngine::kFrame_Buffer_Count> m_bone_matrix_buffer_handles_;

    /// <summary>
    /// WorldMatrix用のBufferがなければ作成し、単位行列を現在のフレームのBufferに書き込みます。頂点の移動はBoneのMatrixで行われます。
    /// </summary>
    void UpdateWorldBuffer() override;
    /// <summary>
    /// 各Boneと親を結ぶ線をGizmosで描画します。
    /// </summary>
    void DrawBones() const;
    /// <summary>
    /// BoneのMatrix用のBufferがなければ作成し、各BoneのWorldMatrixとBindPoseの逆行列から計算したMatrixを書き込みます。
    /// </summary>
    void UpdateBoneTransformsBuffer();
    /// <summary>
    /// boundsの基準となるMatrixとして、RootBoneの親のWorldMatrixを返します。RootBoneがない場合は自身のWorldMatrixを返します。
    /// </summary>
    Matrix BoundsOrigin() override;

public:
    constexpr static int kMaxBonesPerVertex = 4;

    std::vector<std::weak_ptr<Transform>> transforms;
    std::vector<Matrix> inverted_bind_poses;

    AssetPtr<Transform> root_bone;
    
    void OnInspectorGui() override;
    /// <summary>
    /// ShadowMapへの描画の前に、BoneのMatrixを更新してCommandListに設定します。
    /// </summary>
    void UpdateBuffer() override;
    /// <summary>
    /// WorldMatrixとBoneのMatrixを更新し、Meshの描画をRenderPipelineに登録します。
    /// </summary>
    void Render() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<MeshRenderer>(this),
            CEREAL_NVP(m_shared_mesh_),
            CEREAL_NVP(transforms),
            CEREAL_NVP(inverted_bind_poses),
            CEREAL_NVP(root_bone)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::SkinnedMeshRenderer, 1)