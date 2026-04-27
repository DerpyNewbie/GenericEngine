#pragma once
#include "shader_parameter.h"
#include "shader_settings.h"
#include "Rendering/shader_type.h"
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
class Shader : public Object, public Inspectable
{
    friend ShaderImporter;
    static std::shared_ptr<Shader> m_default_shader_;

    ComPtr<ID3DBlob> m_vs_blob_;
    ComPtr<ID3DBlob> m_ps_blob_;
    ComPtr<ID3DBlob> m_gs_blob_;

    ShaderSettings m_shader_settings_;

    void DrawShaderSettings();

public:
    std::vector<ShaderParameter> parameters;

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
            case kShaderType_Geometry:
                if (m_gs_blob_ == nullptr)
                {
                    return {};
                }
                return m_gs_blob_.Get();
            default:
                throw std::runtime_error("Invalid shader type");
        }
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(parameters),
            CEREAL_NVP(m_shader_settings_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Shader, 1)