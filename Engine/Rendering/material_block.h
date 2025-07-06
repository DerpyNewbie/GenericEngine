#pragma once
#include <directx/d3dx12.h>
#include <ranges>
#include "object.h"
#include "shader.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
struct IMaterialData;
struct ShaderParameter;

/// <summary>
/// Shared shader parameters.
/// </summary>
/// <remarks>
/// Used by Material for better memory-management among the same objects
/// </remarks>
class MaterialBlock : public InspectableAsset
{
public:
    MaterialBlock() = default;
    void OnInspectorGui() override;
    void OnConstructed() override;
    
    std::shared_ptr<DescriptorHandle> GetDescriptorHandle(kShaderType shader_type, kParameterBufferType param_buffer);
    std::weak_ptr<IMaterialData> FindMaterialDataFromName(std::string name);
    void CreateParamsFromShaderParams(std::vector<ShaderParameter> shader_params);

    std::array<std::array<std::vector<std::pair<ShaderParameter, std::shared_ptr<IMaterialData>>>, kParameterBufferType_Count>,kShaderType_Count> parameters;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this));
    }
};
}