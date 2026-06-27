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
    static ShaderParameter ReadConstantVariables(ID3D12ShaderReflectionConstantBuffer *cb_reflect, const D3D12_SHADER_INPUT_BIND_DESC &bind_desc);
    static kConstantBufferDataType GetConstantBufferDataType(const D3D12_SHADER_TYPE_DESC &type_desc);
    static void EmplaceShaderParameters(std::vector<ShaderParameter> &base_parameters, const std::vector<ShaderParameter> &src_parameters);
    static void UpdateShaderParameters(const std::shared_ptr<Shader> &shader);
    static bool CompileShader(const std::shared_ptr<Shader> &shader, const std::wstring &file_path, std::string &error_msg);
    static bool CompileBlob(ComPtr<ID3DBlob> &shader_blob, const std::wstring &file_path, const std::string &entry_point, const std::string &target, std::string &error_msg);
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