#pragma once
#include "material_data.h"
#include "shader.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
struct MaterialDataPair
{
    std::shared_ptr<IMaterialData> data = nullptr;

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(data));
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
    
    std::shared_ptr<IMaterialData> FindMaterialDataByName(const std::string &name);

    void UpdateBuffer();
    bool IsDirty();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(material_data)
        );
    }
};

template <typename T>
bool MaterialBlock::SetMaterialData(const std::string &name, T material_data)
{
    for (auto &data : this->material_data | std::views::transform(&MaterialDataPair::data))
    {
        if (data->parameter.name == name)
        {
            auto casted_data = std::dynamic_pointer_cast<MaterialData<T>>(data);
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