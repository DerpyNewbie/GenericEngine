#include "pch.h"
#include "material_block.h"

namespace
{
using namespace engine;

std::shared_ptr<BufferDataBase> CreateMaterialData(const ShaderParameter &shader_param)
{
    switch (shader_param.buffer_type)
    {
    case kBufferType_ConstantBuffer:
        return std::make_shared<ConstantBufferData>(shader_param);
    case kBufferType_StructuredBuffer:
        return std::make_shared<StructuredBufferData>(shader_param);
    case kBufferType_Texture2D:
        return std::make_shared<TextureBufferData>(shader_param);
    case kBufferType_UavTexture:
        return std::make_shared<UavTextureBufferData>(shader_param);
    case kBufferType_ByteAddressBuffer:
        return std::make_shared<ByteAddressBufferData>(shader_param);
    case kBufferType_TextureCube:
        return std::make_shared<TextureCubeBufferData>(shader_param);
    }
}
}

namespace engine
{
void MaterialBlock::OnInspectorGui()
{
    for (auto &[name, data] : m_buffer_data_)
    {
        ImGui::PushID(data.get());
        data->OnInspectorGui();
        ImGui::PopID();
    }
}

std::shared_ptr<ConstantBufferData> MaterialBlock::GetConstantBufferData(const std::string &name)
{
    auto it = m_buffer_data_.find(name);
    if (it == m_buffer_data_.end() || it->second->BufferType() != kBufferType_ConstantBuffer)
        return nullptr;

    return std::reinterpret_pointer_cast<ConstantBufferData>(it->second);
}

std::shared_ptr<StructuredBufferData> MaterialBlock::GetStructuredBufferData(const std::string &name)
{
    auto it = m_buffer_data_.find(name);
    if (it == m_buffer_data_.end() || it->second->BufferType() != kBufferType_StructuredBuffer)
        return nullptr;

    return std::reinterpret_pointer_cast<StructuredBufferData>(it->second);
}

std::shared_ptr<TextureBufferData> MaterialBlock::GetTextureBufferData(const std::string &name)
{
    auto it = m_buffer_data_.find(name);
    if (it == m_buffer_data_.end() || it->second->BufferType() != kBufferType_Texture2D)
        return nullptr;

    return std::reinterpret_pointer_cast<TextureBufferData>(it->second);
}

std::shared_ptr<UavTextureBufferData> MaterialBlock::GetUavTextureBufferData(const std::string &name)
{
    auto it = m_buffer_data_.find(name);
    if (it == m_buffer_data_.end() || it->second->BufferType() != kBufferType_UavTexture)
        return nullptr;

    return std::reinterpret_pointer_cast<UavTextureBufferData>(it->second);
}

std::shared_ptr<ByteAddressBufferData> MaterialBlock::GetByteAddressBufferData(const std::string &name)
{
    auto it = m_buffer_data_.find(name);
    if (it == m_buffer_data_.end())
        return nullptr;

    return std::reinterpret_pointer_cast<ByteAddressBufferData>(it->second);
}

std::shared_ptr<TextureCubeBufferData> MaterialBlock::GetTextureCubeData(const std::string &name)
{
    auto it = m_buffer_data_.find(name);
    if (it == m_buffer_data_.end() || it->second->BufferType() != kBufferType_TextureCube)
        return nullptr;

    return std::reinterpret_pointer_cast<TextureCubeBufferData>(it->second);
}

void MaterialBlock::LoadShaderParameters(const std::vector<ShaderParameter> &shader_params)
{
    for (auto &param : shader_params)
    {
        const auto data = CreateMaterialData(param);

        if (data == nullptr)
            continue;

        switch (param.buffer_type)
        {
        case kBufferType_ConstantBuffer:
            m_buffer_data_.try_emplace(param.name, std::static_pointer_cast<ConstantBufferData>(data));
            break;
        case kBufferType_StructuredBuffer:
            m_buffer_data_.try_emplace(param.name, std::static_pointer_cast<StructuredBufferData>(data));
            break;
        case kBufferType_Texture2D:
            m_buffer_data_.try_emplace(param.name, std::static_pointer_cast<TextureBufferData>(data));
            break;
        case kBufferType_UavTexture:
            m_buffer_data_.try_emplace(param.name, std::static_pointer_cast<UavTextureBufferData>(data));
            break;
        case kBufferType_ByteAddressBuffer:
            m_buffer_data_.try_emplace(param.name, std::static_pointer_cast<ByteAddressBufferData>(data));
            break;
        case kBufferType_TextureCube:
            m_buffer_data_.try_emplace(param.name, std::static_pointer_cast<TextureBufferData>(data));
            break;
        default:
            Logger::Error<MaterialBlock>("Unknown buffer type!");
            break;
        }
    }
}

std::shared_ptr<ConstantBufferData> MaterialBlock::FindConstantBufferData(const std::string &variable_name)
{
    for (const auto &data : m_buffer_data_ | std::views::values)
    {
        if (data == nullptr || data->BufferType() != kBufferType_ConstantBuffer)
            continue;

        auto constant_buffer = std::static_pointer_cast<ConstantBufferData>(data);
        if (constant_buffer->HasVariable(variable_name))
            return constant_buffer;
    }
    return nullptr;
}

void MaterialBlock::ForEachBufferData(
    const std::function<void(const std::string &, BufferDataBase &)> &action) const
{
    for (const auto &[name, data] : m_buffer_data_)
    {
        if (data != nullptr)
            action(name, *data);
    }
}

bool MaterialBlock::ConsumeDirty()
{
    bool is_dirty = false;
    for (const auto &data : m_buffer_data_ | std::views::values)
    {
        if (data != nullptr && data->is_dirty)
        {
            is_dirty = true;
            data->is_dirty = false;
        }
    }
    return is_dirty;
}
}

CEREAL_REGISTER_TYPE(engine::MaterialBlock)