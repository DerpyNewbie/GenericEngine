#include "pch.h"

#include "CabotMaterial.h"

#include "ShaderImporter.h"

CabotMaterial::CabotMaterial()
{
    SetShader(ShaderImporter::GetDefault());
}

void CabotMaterial::SetShader(ShaderObject shaderObject)
{
    for (auto materialValue : shaderObject.MaterialValues)
    {
        switch (materialValue.second)
        {
        case ShaderObject::ValueType::Float:
            m_Values.emplace(shaderObject.Name, 0.5f);
            continue;
        case ShaderObject::ValueType::Texture:
            m_Textures.emplace(shaderObject.Name, Texture2D::GetWhite());
            continue;
        case ShaderObject::ValueType::Color:
            //TODO:追加するべし
        default:
            continue;
        }
    }
}

void CabotMaterial::SetTexture(const std::string &string, const std::shared_ptr<Texture2D> &tex)
{
    m_Textures[string] = tex;
}

std::vector<float> CabotMaterial::Values()
{
    std::vector<float> result;
    for (auto value : m_Values)
    {
        result.emplace_back(value.second);
    }
    return result;
}

std::vector<std::shared_ptr<Texture2D>> CabotMaterial::Textures()
{
    std::vector<std::shared_ptr<Texture2D>> result;
    for (auto texture : m_Textures)
    {
        result.emplace_back(texture.second);
    }
    return result;
}