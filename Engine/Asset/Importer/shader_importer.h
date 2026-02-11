#pragma once
#include "asset_importer.h"
#include "Rendering/shader_parameter.h"

namespace engine
{
class Shader;

class ShaderImporter : public AssetImporter
{
    static constexpr auto kShaderMetaVersionKey = "shader_meta_version";
    static constexpr auto kShaderMetaKey = "shader_meta";

    static std::vector<std::shared_ptr<ShaderParameter>> ParseShaderParameters(const std::shared_ptr<Shader> &shader);
    static std::vector<std::shared_ptr<ShaderParameter>> ParseConstantBufferShaderVariables(ID3D12ShaderReflectionConstantBuffer *reflection_buffer, const ComPtr<ID3D12ShaderReflection> &reflector);
    static std::shared_ptr<ShaderParameter> ParseTextureBufferShaderVariables(D3D12_SHADER_INPUT_BIND_DESC *bind_desc, const ComPtr<ID3D12ShaderReflection> &reflector);
    static std::shared_ptr<ShaderParameter> ParseShaderVariable(int register_idx, ID3D12ShaderReflectionVariable *variable);
    static std::shared_ptr<ShaderParameter> ParseShaderVariable(int register_idx, D3D12_SHADER_INPUT_BIND_DESC *bind_desc);
    static std::string GetTypeHint(ID3D12ShaderReflectionVariable *variable);
    static std::string GetTypeHint(const D3D12_SHADER_INPUT_BIND_DESC *bind_desc);
    static void CreateShaderParameters(const std::shared_ptr<Shader> &shader);
    static bool CompileShader(const std::shared_ptr<Shader> &shader, const std::wstring &file_path);
    bool LoadOldParameters(const std::shared_ptr<Shader> &shader, AssetDescriptor *descriptor) const;
    static bool WriteShaderMeta(const std::shared_ptr<Shader> &shader, PersistentDataStore data_store);

public:
    static constexpr int kShaderMetaVersion = 4;
    
    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;
    void OnExport(AssetDescriptor *ctx) override;
};
}