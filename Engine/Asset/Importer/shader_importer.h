#pragma once
#include "asset_importer.h"

namespace engine
{
class Shader;

class ShaderImporter : public AssetImporter
{
    static bool CompileShader(const std::shared_ptr<Shader> &shader, const std::wstring &file_path);
    bool LoadParameters(const std::shared_ptr<Shader> &shader, AssetDescriptor *descriptor) const;

public:
    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;
    void OnExport(AssetDescriptor *ctx) override;
};
}