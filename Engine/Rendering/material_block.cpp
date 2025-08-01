#include "pch.h"
#include "material_block.h"
#include "material_data.h"
#include "world_view_projection.h"
#include "Asset/Importer/texture_2d_importer.h"

namespace
{
using namespace engine;
using MaterialFactory =
std::function<std::shared_ptr<IMaterialData>(const ShaderParameter &)>;

std::unordered_map<std::string, MaterialFactory> g_material_data_factory = {
    {"int", [](const ShaderParameter &param) {
        return std::make_shared<MaterialData<int>>(0, param);
    }},
    {"float", [](const ShaderParameter &param) {
        return std::make_shared<MaterialData<float>>(0, param);
    }},
    {"__WVP__", [](const ShaderParameter &param) {
        auto wvp = WorldViewProjection{};
        for (int i = 0; i < 3; ++i)
            wvp.WVP[i] = Matrix::Identity;
        return std::make_shared<MaterialData<WorldViewProjection>>(wvp, param);
    }},
    {"vector<Matrix>", [](const ShaderParameter &param) {
        auto identity = std::vector{Matrix::Identity};
        return std::make_shared<MaterialData<std::vector<Matrix>>>(identity, param);
    }},
    {"Texture2D", [](const ShaderParameter &param) {
        auto texture = Texture2DImporter::GetColorTexture({1, 0, 1, 1});
        return std::make_shared<MaterialData<AssetPtr<Texture2D>>>(
            AssetPtr<Texture2D>::FromIAssetPtr(texture),
            param);
    }}
};

std::shared_ptr<IMaterialData> CreateMaterialData(const std::weak_ptr<ShaderParameter> &shader_param)
{
    const auto func = g_material_data_factory[shader_param.lock()->type_hint];
    auto material_data = func(*shader_param.lock());
    return material_data;
}
}

namespace engine
{
int *ShaderDataIndex::GetLengthField(kParameterBufferType type)
{
    switch (type)
    {
    case kParameterBufferType_CBV:
        return &cbv_length;
    case kParameterBufferType_SRV:
        return &srv_length;
    case kParameterBufferType_UAV:
        return &uav_length;
    default:
        static_assert("Invalid buffer type");
        return nullptr;
    }
}

int ShaderDataIndex::GetLength(const kParameterBufferType type) const
{
    switch (type)
    {
    case kParameterBufferType_CBV:
        return cbv_length;
    case kParameterBufferType_SRV:
        return srv_length;
    case kParameterBufferType_UAV:
        return uav_length;
    default:
        static_assert("Invalid buffer type");
        return 0;
    }
}

int ShaderDataIndex::GetOffset(kParameterBufferType type) const
{
    int offset = 0;

    // fall-through
    switch (type)
    {
    case kParameterBufferType_UAV:
        offset += srv_length;
    case kParameterBufferType_SRV:
        offset += cbv_length;
    case kParameterBufferType_CBV:
    default:
        break;
    }

    return offset;
}

int ShaderDataIndex::GetFullLength() const
{
    return cbv_length + srv_length + uav_length;
}

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

void MaterialBlock::LoadShaderParameters(const std::vector<std::shared_ptr<ShaderParameter>> &shader_params)
{
    for (auto &param : shader_params)
    {
        const auto material_data = CreateMaterialData(param);
        Insert(material_data);
    }
    UpdateBuffer();
}

ShaderDataIndex *MaterialBlock::GetShaderDataIndex(const kShaderType type)
{
    switch (type)
    {
    case kShaderType_Vertex:
        return &vertex_shader_index;
    case kShaderType_Pixel:
        return &pixel_shader_index;
    default:
        static_assert("Unreachable");
        return nullptr;
    }
}

int MaterialBlock::GetOffset(const kShaderType type) const
{
    switch (type)
    {
    case kShaderType_Pixel:
        return 0;
    case kShaderType_Vertex:
        return pixel_shader_index.GetFullLength();
    default:
        static_assert("Unreachable");
        return 0;
    }
}

void MaterialBlock::Insert(const std::shared_ptr<IMaterialData> &data)
{
    Logger::Log<MaterialBlock>("Inserting data %s", data->parameter.name.c_str());

    const auto shader_type = data->parameter.shader_type;
    const auto buffer_type = data->BufferType();
    data->CreateBuffer();
    material_data.insert(End(shader_type, buffer_type), {data, nullptr});
    data->is_dirty = false;

    const auto field = GetShaderDataIndex(shader_type)->GetLengthField(buffer_type);
    ++(*field);
}

bool MaterialBlock::Empty(const kShaderType shader_type, const kParameterBufferType buffer_type)
{
    return GetShaderDataIndex(shader_type)->GetLength(buffer_type) == 0;
}

std::vector<MaterialDataPair>::iterator MaterialBlock::Begin(const kShaderType shader_type,
                                                             const kParameterBufferType buffer_type)
{
    const auto shader_offset = GetOffset(shader_type);
    const auto buffer_offset = GetShaderDataIndex(shader_type)->GetOffset(buffer_type);
    return material_data.begin() + shader_offset + buffer_offset;
}

std::vector<MaterialDataPair>::iterator MaterialBlock::End(const kShaderType shader_type,
                                                           const kParameterBufferType buffer_type)
{
    return Begin(shader_type, buffer_type) + GetShaderDataIndex(shader_type)->GetLength(buffer_type);
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