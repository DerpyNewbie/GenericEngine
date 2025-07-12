#pragma once
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
    bool m_IsCreate_Buffer_ = false;

public:
    MaterialBlock() = default;
    void OnInspectorGui() override;
    void OnConstructed() override;

    std::shared_ptr<DescriptorHandle> GetDescriptorHandle(kShaderType shader_type, kParameterBufferType param_buffer);
    std::weak_ptr<IMaterialData> FindMaterialDataFromName(std::string name);
    void CreateParamsFromShaderParams(std::vector<std::shared_ptr<ShaderParameter>> shader_params);
    void CreateBuffer();
    bool IsCreateBuffer();

    std::array<std::array<std::vector<std::shared_ptr<IMaterialData>>, kParameterBufferType_Count>, kShaderType_Count>
    material_datasets;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this));
    }
};
}