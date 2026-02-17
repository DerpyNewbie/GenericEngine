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
    void OnConstructed() override;
    void CreateMaterialBlock();

    void UpdateBuffer();
    bool IsDirty() const;

    void SetDescriptorTable();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(p_shared_material_block)
        );

        if (version >= 2)
        {
            ar(
                CEREAL_NVP(render_queue),
                CEREAL_NVP(shader)
            );
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::Material, 2)