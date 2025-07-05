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
class Material : public Object
{
public:
    std::shared_ptr<Shader> shared_shader;
    std::shared_ptr<MaterialBlock> shared_material_block;

    void OnConstructed() override;
    void CreateMaterialBlock();
    
    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this), shared_material_block);
    }
};
}