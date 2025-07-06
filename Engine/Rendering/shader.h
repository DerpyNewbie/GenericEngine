#pragma once
#include "Asset/inspectable_asset.h"
#include "Asset/Importer/ShaderImporter.h"
#include "CabotEngine/Graphics/ComPtr.h"

namespace engine
{

enum kParameterBufferType
{
    kParameterBufferType_CBV,
    kParameterBufferType_SRV,
    kParameterBufferType_UAV,

    kParameterBufferType_Count
};

enum kShaderType
{
    kShaderType_Vertex,
    kShaderType_Pixel,

    kShaderType_Count
};


struct ShaderParameter
{
    std::string name;
    int index;
    kShaderType shader_type;
    kParameterBufferType buffer_type;
    std::string type_hint;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(name, index, shader_type, buffer_type, type_hint);
    }
};

/// <summary>
/// Shader object representation
/// </summary>
/// <remarks>
/// Base of all shaders used in the engine. Shaders such as PS, VS inherits from here.
/// </remarks>
class Shader : public InspectableAsset
{
    friend ShaderImporter;
    ComPtr<ID3DBlob> m_pVSBlob;
    ComPtr<ID3DBlob> m_pPSBlob;

public:
    enum class ShaderType
    {
        Vertex,
        Pixel
    };

    static std::shared_ptr<Shader> default_shader;
    std::vector<ShaderParameter> parameters;

    static std::shared_ptr<Shader> GetDefault();

    void OnInspectorGui() override;
    void OnConstructed() override;

    CD3DX12_SHADER_BYTECODE GetByteCode(ShaderType type) const
    {
        switch (type)
        {
        case ShaderType::Vertex:
            return m_pVSBlob.Get();
        case ShaderType::Pixel:
            return m_pPSBlob.Get();
        }
    }
};

}