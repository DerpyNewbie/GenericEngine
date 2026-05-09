#pragma once
#include "material_data.h"
#include "shader.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
struct MaterialDataPair
{
    std::shared_ptr<MaterialData> data = nullptr;
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

    int *GetLengthField(kGpuUploadType type);
    int GetLength(kGpuUploadType type) const;
    int GetOffset(kGpuUploadType type) const;
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
class MaterialBlock : public Object, public Inspectable
{
public:
    ShaderDataIndex shader_index = {};

    std::vector<MaterialDataPair> material_data = {};

    MaterialBlock() = default;
    ~MaterialBlock() override;

    void OnInspectorGui() override;

    template <typename T>
    bool SetMaterialData(const std::string &name, T material_data);

    void LoadShaderParameters(
        const std::vector<ShaderParameter> &shader_params,
        const std::vector<MaterialDataPair> &resource_material_data = {}
    );

    void Insert(const std::shared_ptr<MaterialData> &data);
    bool Empty(kGpuUploadType buffer_type);
    std::vector<MaterialDataPair>::iterator Begin(kGpuUploadType buffer_type);
    std::vector<MaterialDataPair>::iterator End(kGpuUploadType buffer_type);

    std::shared_ptr<MaterialData> FindMaterialDataByName(const std::string &name);

    void UpdateBuffer();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(material_data)
        );

        if (version >= 2)
        {
            ar(
                CEREAL_NVP(shader_index)
            );
        }
    }
};

template <typename T>
bool MaterialBlock::SetMaterialData(const std::string &name, T material_data)
{
    for (auto &data : this->material_data | std::views::transform(&MaterialDataPair::data))
    {
        if (data->parameter.name == name)
        {
            auto casted_data = std::dynamic_pointer_cast<MaterialData>(data);
            if (casted_data == nullptr)
                return false;
            casted_data->SetValue(material_data);
            return true;
        }
    }
    return false;
}
}

CEREAL_CLASS_VERSION(engine::MaterialBlock, 2)