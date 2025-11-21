#pragma once
#include "shader_parameter.h"
#include "shader_settings.h"
#include "Asset/inspectable_asset.h"
#include "CabotEngine/Graphics/ComPtr.h"

namespace engine
{
class ShaderImporter;

/// <summary>
/// Shader object representation
/// </summary>
/// <remarks>
/// Base of all shaders used in the engine. Shaders such as PS, VS inherits from here.
/// </remarks>
class Shader : public InspectableAsset
{
    friend ShaderImporter;
    static std::shared_ptr<Shader> m_default_shader_;

    bool m_is_transparent_ = false;
    ComPtr<ID3DBlob> m_vs_blob_;
    ComPtr<ID3DBlob> m_ps_blob_;

    ShaderSettings m_shader_settings_;

    void DrawShaderSettings();
public:
    std::vector<std::shared_ptr<ShaderParameter>> parameters;

    void OnInspectorGui() override;
    ShaderSettings ShaderSettings() const;
    static std::shared_ptr<Shader> GetDefault();

    CD3DX12_SHADER_BYTECODE GetByteCode(const kShaderType type) const
    {
        switch (type)
        {
        case kShaderType_Vertex:
            return m_vs_blob_.Get();
        case kShaderType_Pixel:
            return m_ps_blob_.Get();
        default:
            throw std::runtime_error("Invalid shader type");
        }
    }
};

}