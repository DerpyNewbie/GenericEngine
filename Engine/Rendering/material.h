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
class Material : public Object
{
public:
    // TODO: should vertex shader be swappable? 
    std::shared_ptr<VertexShader> vertex_shader;
    std::shared_ptr<PixelShader> pixel_shader;
    std::shared_ptr<MaterialBlock> shared_material_block;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this), vertex_shader, pixel_shader, shared_material_block);
    }
};
}