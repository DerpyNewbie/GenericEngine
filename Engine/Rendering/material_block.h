#pragma once
#include "material_data.h"
#include "shader.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
struct MaterialDataPair
{
    std::shared_ptr<IMaterialData> data = nullptr;
    std::shared_ptr<DescriptorHandle> handle = nullptr;

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(data));
    }
};

struct ShaderDataIndex
{
    int cbv_length = 0;
    int srv_length = 0;
    int uav_length = 0;

    int *GetLengthField(kParameterBufferType type);
    int GetLength(kParameterBufferType type) const;
    int GetOffset(kParameterBufferType type) const;
    int GetFullLength() const;

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(cbv_length), CEREAL_NVP(srv_length), CEREAL_NVP(uav_length));
    }
};

/// <summary>
/// Shared shader parameters.
/// </summary>
/// <remarks>
/// Used by Material for better memory-management among the same objects
/// </remarks>
class MaterialBlock : public InspectableAsset
{
public:
    ShaderDataIndex pixel_shader_index = {};
    ShaderDataIndex vertex_shader_index = {};

    std::vector<MaterialDataPair> material_data = {};

    MaterialBlock() = default;
    ~MaterialBlock() override;

    void OnInspectorGui() override;

    void LoadShaderParameters(const std::vector<std::shared_ptr<ShaderParameter>> &shader_params);

    ShaderDataIndex *GetShaderDataIndex(kShaderType type);
    int GetOffset(kShaderType type) const;

    void Insert(const std::shared_ptr<IMaterialData> &data);
    bool Empty(kShaderType shader_type, kParameterBufferType buffer_type);
    std::vector<MaterialDataPair>::iterator Begin(kShaderType shader_type, kParameterBufferType buffer_type);
    std::vector<MaterialDataPair>::iterator End(kShaderType shader_type, kParameterBufferType buffer_type);

    std::shared_ptr<IMaterialData> FindMaterialDataByName(const std::string &name);

    void UpdateBuffer();
    bool IsDirty();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this),
           CEREAL_NVP(material_data), CEREAL_NVP(pixel_shader_index), CEREAL_NVP(vertex_shader_index));
    }
};
}