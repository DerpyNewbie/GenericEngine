#pragma once
#include "material_block.h"
#include "shader.h"
#include <assimp/scene.h>

namespace engine
{

/// <summary>
/// A Material that can be applied to Renderers.
/// </summary>
/// <remarks>
/// Contains all necessary information for render pass on the Renderer, except for the actual Mesh and Bone transform information.
/// </remarks>
class Material : public InspectableAsset
{
public:
    AssetPtr<Shader> p_shared_shader;
    std::shared_ptr<MaterialBlock> p_shared_material_block;

    void OnInspectorGui() override;
    void OnConstructed() override;
    void CreateMaterialBlock();

    void UpdateBuffer();
    bool IsDirty() const;
    bool IsValid() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this), CEREAL_NVP(p_shared_shader), CEREAL_NVP(p_shared_material_block));
    }
};
}