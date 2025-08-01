#pragma once
#include "shader_parameter.h"
#include "Asset/inspectable_asset.h"
#include "CabotEngine/Graphics/ComPtr.h"

namespace engine
{
class ShaderImporter;

enum kShaderType
{
    kShaderType_Vertex,
    kShaderType_Pixel,

    kShaderType_Count
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
    static std::shared_ptr<Shader> m_pDefaultShader;

public:
    enum class ShaderType
    {
        Vertex,
        Pixel
    };

    std::vector<std::shared_ptr<ShaderParameter>> parameters;


    void OnInspectorGui() override;
    static std::shared_ptr<Shader> GetDefault();

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