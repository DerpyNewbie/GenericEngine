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
public:
    uint16_t render_queue = 5000;
    AssetPtr<Shader> shader;
    std::shared_ptr<MaterialBlock> p_shared_material_block;

    void OnInspectorGui() override;
    /// <summary>
    /// BasicShaderを設定し、MaterialBlockを作成します。
    /// </summary>
    void OnConstructed() override;
    /// <summary>
    /// Shaderのparametersに合わせて、MaterialBlockを作成します。
    /// </summary>
    void CreateMaterialBlock();

    /// <summary>
    /// MaterialBlockのBufferを更新します。MaterialBlockがない場合は作成します。
    /// </summary>
    void UpdateBuffer();
    /// <summary>
    /// まだBufferに反映されていない変更があるかどうかを取得します。
    /// </summary>
    bool IsDirty() const;

    /// <summary>
    /// Bufferを更新し、CBV、SRV、UAVそれぞれのDescriptorTableをCommandListに設定します。
    /// </summary>
    void SetDescriptorTable();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(p_shared_material_block)
        );

        if (version >= 3)
        {
            ar(
                CEREAL_NVP(render_queue),
                CEREAL_NVP(shader)
            );
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::Material, 3)