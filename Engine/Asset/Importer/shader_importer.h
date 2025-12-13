#pragma once
#include "asset_importer.h"

namespace engine
{
class Shader;

class ShaderImporter : public AssetImporter
{
    static constexpr auto kShaderMetaVersionKey = "shader_meta_version";
    static constexpr auto kShaderMetaKey = "shader_meta";

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