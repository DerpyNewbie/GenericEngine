#include "Rendering/CaboRenderingSystem/Material/material.h"

#include <algorithm>
#include <cstring>

#include "Rendering/CaboRenderingSystem/Core/log.h"

namespace CRS
{
void Material::SetPass(const PassTag tag, const MaterialPass &pass)
{
    m_passes_[static_cast<size_t>(tag)] = pass;
    ++m_version_;
}

void Material::RemovePass(const PassTag tag)
{
    m_passes_[static_cast<size_t>(tag)].reset();
    ++m_version_;
}

const MaterialPass *Material::GetPass(const PassTag tag) const
{
    const auto &pass = m_passes_[static_cast<size_t>(tag)];
    return pass.has_value() ? &*pass : nullptr;
}

void Material::BuildPropertyTable(const IRenderDevice &device)
{
    const auto forward = GetPass(PassTag::Forward);
    if (forward == nullptr)
    {
        LogWarning("BuildPropertyTable: material has no Forward pass");
        return;
    }

    const auto old_properties = std::move(m_properties_);
    const auto old_constants = std::move(m_constants_);
    m_properties_.clear();
    m_texture_slots_.clear();
    m_sampler_slots_.clear();

    for (auto &constants : m_constants_)
        constants.clear();

    std::array<uint32_t, kConstantSlotCount> constant_sizes = {};
    for (const auto shader : {forward->vertex_shader, forward->pixel_shader})
    {
        if (!shader.IsValid())
            continue;

        const auto &reflection = device.GetReflection(shader);
        for (const auto &cb : reflection.constant_buffers)
        {
            // space1はエンジン用なので、マテリアルの表には入れない
            if (cb.space != 0)
                continue;
            if (cb.slot >= kConstantSlotCount)
            {
                LogWarning("cbuffer '{}' uses b{}, but materials support up to b{}", cb.name, cb.slot,
                           kConstantSlotCount - 1);
                continue;
            }

            constant_sizes[cb.slot] = std::max(constant_sizes[cb.slot], cb.size);
            for (const auto &variable : cb.variables)
                m_properties_[variable.name] = {cb.slot, variable.offset, variable.size, variable.type};
        }

        for (const auto &resource : reflection.resources)
        {
            // space1はフレーム・オブジェクト用なので、マテリアルの表には入れない
            if (resource.space != 0)
                continue;

            if (resource.kind == ShaderResourceKind::Sampler)
                m_sampler_slots_[resource.name] = resource.slot;
            else if (resource.kind == ShaderResourceKind::Texture)
                m_texture_slots_[resource.name] = resource.slot;
        }
    }

    for (uint32_t slot = 0; slot < kConstantSlotCount; ++slot)
        m_constants_[slot].resize(constant_sizes[slot]);

    // 名前と型が同じ値は引き継ぐ
    for (const auto &[name, property] : m_properties_)
    {
        const auto old = old_properties.find(name);
        if (old == old_properties.end() || old->second.type != property.type || old->second.size != property.size)
            continue;

        const auto &old_data = old_constants[old->second.slot];
        if (old->second.offset + old->second.size <= old_data.size())
            std::memcpy(m_constants_[property.slot].data() + property.offset, old_data.data() + old->second.offset,
                        property.size);
    }
}

void Material::BindConstants(Bindings &bindings) const
{
    for (uint32_t slot = 0; slot < kConstantSlotCount; ++slot)
    {
        const auto &constants = m_constants_[slot];
        if (!constants.empty())
            bindings.constants[slot] = ConstantBinding::FromData(constants.data(),
                                                                 static_cast<uint32_t>(constants.size()));
    }
}

bool Material::Write(const std::string &name, const ShaderVariableType type, const void *data, const uint32_t size)
{
    const auto itr = m_properties_.find(name);
    if (itr == m_properties_.end())
        return false;

    const auto &property = itr->second;
    auto &constants = m_constants_[property.slot];
    if (property.type != type || property.size < size || property.offset + size > constants.size())
    {
        LogWarning("Material property '{}' type mismatch", name);
        return false;
    }

    std::memcpy(constants.data() + property.offset, data, size);
    return true;
}

bool Material::SetInt(const std::string &name, const int32_t value)
{
    return Write(name, ShaderVariableType::Int, &value, sizeof(value));
}

bool Material::SetFloat(const std::string &name, const float value)
{
    return Write(name, ShaderVariableType::Float, &value, sizeof(value));
}

bool Material::SetVector(const std::string &name, const Vector4 &value)
{
    return Write(name, ShaderVariableType::Float4, &value, sizeof(value));
}

bool Material::SetColor(const std::string &name, const Color &value)
{
    return Write(name, ShaderVariableType::Float4, &value, sizeof(value));
}

bool Material::SetMatrix(const std::string &name, const Matrix &value)
{
    return Write(name, ShaderVariableType::Float4x4, &value, sizeof(value));
}

bool Material::SetData(const std::string &name, const void *data, const uint32_t size)
{
    const auto itr = m_properties_.find(name);
    if (itr == m_properties_.end())
        return false;

    const auto &property = itr->second;
    auto &constants = m_constants_[property.slot];
    const auto copy_size = std::min(size, property.size);
    if (property.offset + copy_size > constants.size())
        return false;

    std::memcpy(constants.data() + property.offset, data, copy_size);
    return true;
}

bool Material::SetTexture(const std::string &name, const TextureHandle texture)
{
    const auto itr = m_texture_slots_.find(name);
    if (itr == m_texture_slots_.end() || itr->second >= kResourceSlotCount)
        return false;

    m_textures_[itr->second] = texture;
    return true;
}

bool Material::SetSampler(const std::string &name, const SamplerHandle sampler)
{
    const auto itr = m_sampler_slots_.find(name);
    if (itr == m_sampler_slots_.end() || itr->second >= kSamplerSlotCount)
        return false;

    m_samplers_[itr->second] = sampler;
    return true;
}

bool Material::HasProperty(const std::string &name) const
{
    return m_properties_.contains(name);
}

bool Material::HasTexture(const std::string &name) const
{
    return m_texture_slots_.contains(name);
}

MaterialId MaterialRegistry::Add(Material material)
{
    return m_materials_.Add(std::move(material));
}

void MaterialRegistry::Remove(const MaterialId id)
{
    m_materials_.Remove(id);
}

Material *MaterialRegistry::Get(const MaterialId id)
{
    return m_materials_.Get(id);
}

const Material *MaterialRegistry::Get(const MaterialId id) const
{
    return m_materials_.Get(id);
}

size_t MaterialRegistry::Count() const
{
    return m_materials_.Count();
}
}