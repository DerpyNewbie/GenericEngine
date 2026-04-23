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

    static std::vector<ShaderParameter> ReadShaderParameters(const std::shared_ptr<Shader> &shader);
    static std::vector<ShaderParameter> ReadShaderBlob(const ComPtr<ID3D10Blob> &shader_blob);
    static std::vector<ShaderParameter> ReadConstantBufferVariables(const ComPtr<ID3D12ShaderReflection> &shader, ID3D12ShaderReflectionConstantBuffer *constant_buffer);
    static ShaderParameter ConvertToShaderParameter(const D3D12_SHADER_INPUT_BIND_DESC *bind_desc);
    static ShaderParameter ConvertToShaderParameter(UINT register_idx, const D3D12_SHADER_VARIABLE_DESC &variable_desc, const D3D12_SHADER_TYPE_DESC &type_desc);
    static std::string GetTypeHint(const D3D12_SHADER_TYPE_DESC &type_desc);
    static std::string GetTypeHint(const D3D12_SHADER_INPUT_BIND_DESC *bind_desc);
    static void UpdateShaderParameters(const std::shared_ptr<Shader> &shader);
    static std::string CompileShader(const std::shared_ptr<Shader> &shader, const std::wstring &file_path);
    static bool WriteShaderMeta(const std::shared_ptr<Shader> &shader, PersistentDataStore data_store);
    static bool IsReservedBufferName(std::string_view buffer_name);

public:
    static constexpr int kShaderMetaVersion = 4;

    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;
    void OnExport(AssetDescriptor *ctx) override;
};
}