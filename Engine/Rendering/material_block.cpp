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
MaterialBlock::~MaterialBlock()
{
    for (auto &desc_handle : material_data | std::views::transform(&MaterialDataPair::handle))
    {
        DescriptorHeap::Free(desc_handle);
        desc_handle = nullptr;
    }
}

void MaterialBlock::OnInspectorGui()
{
    for (auto &data : material_data | std::views::transform(&MaterialDataPair::data))
    {
        ImGui::PushID(data.get());
        data->OnInspectorGui();
        ImGui::PopID();
    }
}

void MaterialBlock::LoadShaderParameters(
    const std::vector<ShaderParameter> &shader_params,
    const std::vector<MaterialDataPair> &resource_material_data
)
{
    for (auto &param : shader_params)
    {
        bool found = false;
        if (!resource_material_data.empty())
        {
            for (auto material_data_pair : resource_material_data)
            {
                if (param.name == material_data_pair.data->parameter.name)
                {
                    Insert(material_data_pair.data);
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
                Insert(material_data);
            }
        }
    }
    UpdateBuffer();
}

std::shared_ptr<IMaterialData> MaterialBlock::FindMaterialDataByName(const std::string &name)
{
    for (auto &data : material_data | std::views::transform(&MaterialDataPair::data))
    {
        if (data->parameter.name == name)
            return data;
    }

    return {};
}

void MaterialBlock::UpdateBuffer()
{
    for (auto &[data, handle] : material_data)
    {
        if (data->is_dirty)
        {
            Logger::Log<MaterialBlock>("Updating data in MaterialBlock: %s", data->parameter.name.c_str());

            if (data->buffer == nullptr)
            {
                data->buffer = data->CreateBuffer();
            }

            if (data->CanUpdateBuffer())
            {
                data->UpdateBuffer();
            }
            else
            {
                if (handle != nullptr)
                {
                    DescriptorHeap::Free(handle);
                    handle = nullptr;
                }
            }

            data->is_dirty = false;
        }

        if (handle == nullptr)
        {
            handle = data->UploadBuffer();
        }
    }
}

bool MaterialBlock::IsDirty()
{
    for (const auto &data : material_data | std::views::transform(&MaterialDataPair::data))
    {
        if (data->is_dirty)
        {
            return true;
        }
    }

    return false;
}
}

CEREAL_REGISTER_TYPE(engine::MaterialBlock)