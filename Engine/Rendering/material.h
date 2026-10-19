#pragma once
#include "material_block.h"
#include "shader.h"

namespace engine
{

/// <summary>
/// A Material that can be applied to Renderers.
/// </summary>
/// <remarks>
/// Contains all necessary information for render pass on the Renderer, except for the actual Mesh and Bone transform information.
/// </remarks>
class Material : public Object, public Inspectable
{
    AssetPtr<Shader> m_shader_;

public:

    uint16_t render_queue = 5000;
    std::shared_ptr<MaterialBlock> shared_material_block;
    
    void OnInspectorGui() override;
    void OnConstructed() override;
    /// <summary>
    /// 設定されているShaderのパラメーターから、MaterialBlockを新しく作成します。
    /// </summary>
    void CreateMaterialBlock();

    /// <summary>
    /// 設定されているShaderを取得します。
    /// </summary>
    AssetPtr<Shader> GetShader();

    /// <summary>
    /// MaterialBlockが作成されていなければ作成します。
    /// </summary>
    void UpdateBuffer();
    /// <summary>
    /// MaterialBlockが未作成で、作り直しが必要かどうかを取得します。
    /// </summary>
    bool IsDirty() const;

    /// <summary>
    /// Shaderを設定し、MaterialBlockを作り直します。
    /// </summary>
    /// <param name="shader">設定するShader</param>
    void SetShader(const AssetPtr<Shader> &shader);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(shared_material_block)
        );

        if (version >= 3)
        {
            ar(
                CEREAL_NVP(render_queue),
                CEREAL_NVP(m_shader_)
            );
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::Material, 3)