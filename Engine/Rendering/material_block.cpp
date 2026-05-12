#include "pch.h"
#include "material_block.h"
#include "material_data.h"
#include "Asset/Importer/texture_2d_importer.h"

namespace
{
using namespace engine;
using MaterialFactory =
std::function<std::shared_ptr<IMaterialData>(const ShaderParameter &)>;

std::unordered_map<std::string, MaterialFactory> g_material_data_factory = {
    {"int",
     [](const ShaderParameter &param) {
         return std::make_shared<MaterialData<int>>(0, param);
     }
    },
    {"float",
     [](const ShaderParameter &param) {
         return std::make_shared<MaterialData<float>>(0.0F, param);
     }
    },
    {"color",
     [](const ShaderParameter &param) {
         return std::make_shared<MaterialData<Color>>(Color(), param);
     }
    },
    {"float2",
     [](const ShaderParameter &param) {
         return std::make_shared<MaterialData<Vector2>>(Vector2::Zero, param);
     }
    },
    {"float3",
     [](const ShaderParameter &param) {
         return std::make_shared<MaterialData<Vector3>>(Vector3::Zero, param);
     }
    },
    {"texture2d",
     [](const ShaderParameter &param) {
         auto texture = Texture2DImporter::GetColorTexture({0.7f, 0.7f, 0.7f, 1.0f});
         return std::make_shared<MaterialData<AssetPtr<Texture2D>>>(
             AssetPtr<Texture2D>::FromIAssetPtr(texture),
             param
         );
     }
    }
};

std::shared_ptr<IMaterialData> CreateMaterialData(const ShaderParameter &shader_param)
{
    const auto func = g_material_data_factory[shader_param.type_hint];
    if (func != nullptr)
    {
        auto material_data = func(shader_param);
        return material_data;
    }
    return nullptr;
}
}

namespace engine
{

void MaterialBlock::OnInspectorGui()
{
    for (auto &data : m_material_data_)
    {
        ImGui::PushID(data.get());
        data->OnInspectorGui();
        ImGui::PopID();
    }
}

void MaterialBlock::LoadShaderParameters(
    const std::vector<ShaderParameter> &shader_params,
    const std::vector<std::shared_ptr<IMaterialData>> &resource_material_data
)
{

    //TODO : パラメーターの順番が正しいとは限らないので、パラメーターのインデックス見に行った方がいいかも。 要検証。
    for (auto &param : shader_params)
    {
        bool found = false;
        if (!resource_material_data.empty())
        {
            for (auto material_data : resource_material_data)
            {
                if (param.name == material_data->parameter.name)
                {
                    m_material_data_.emplace_back(material_data);
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            const auto material_data = CreateMaterialData(param);

            if (material_data != nullptr)
            {
                m_material_data_.emplace_back(material_data);
            }
        }
    }
}

std::shared_ptr<IMaterialData> MaterialBlock::FindMaterialDataByName(const std::string &name)
{
    const auto it = std::ranges::find_if(m_material_data_, [&name](const std::shared_ptr<IMaterialData> &data) {
        )
        return data->parameter.name == name;
    });
    if (it != m_material_data_.end())
    {
        return *it;
    }

    return {};
}

bool MaterialBlock::IsDirty()
{
    const auto it = std::ranges::find_if(m_material_data_, [](const std::shared_ptr<IMaterialData> &data) {
        return data->is_dirty;
    });

    if (it != m_material_data_.end())
    {
        return true;
    }

    return false;
}
}

CEREAL_REGISTER_TYPE(engine::MaterialBlock)