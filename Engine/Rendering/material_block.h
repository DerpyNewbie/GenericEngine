#pragma once
#include "shader.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

using DescriptorHandlePerBuffer = std::array<std::vector<std::shared_ptr<DescriptorHandle>>,
                                             engine::kParameterBufferType_Count>;
using DescriptorHandlePerShader = std::array<DescriptorHandlePerBuffer, engine::kShaderType_Count>;

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

    void CreateDescHandles();
    void ReleaseDescHandles();

public:
    std::array<std::array<std::vector<std::shared_ptr<IMaterialData>>, kParameterBufferType_Count>, kShaderType_Count>
    material_datasets;

    DescriptorHandlePerShader desc_handles;

    MaterialBlock() = default;
    ~MaterialBlock() override;
    void OnInspectorGui() override;
    void OnConstructed() override;

    std::weak_ptr<IMaterialData> FindMaterialDataFromName(std::string name);
    void CreateParamsFromShaderParams(std::vector<std::shared_ptr<ShaderParameter>> shader_params);
    void CreateBuffer();
    bool IsCreateBuffer();
    DescriptorHandlePerShader GetHandles();


    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this));
    }
};
}