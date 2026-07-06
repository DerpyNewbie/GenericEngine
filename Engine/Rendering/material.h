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
    void CreateMaterialBlock();

    AssetPtr<Shader> GetShader();
    
    void UpdateBuffer();
    bool IsDirty() const;

    //これを作ったらBillboardが使えるようになるのでそれを使ってUAVが正しく出来ているかどうかチェックしてください
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