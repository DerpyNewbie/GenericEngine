#pragma once
#include "asset_importer.h"

namespace engine
{
class Shader;

class ShaderImporter : public AssetImporter
{
    bool CompileShader(std::shared_ptr<Shader> shader, std::wstring file_path);
public:
    std::vector<std::string>SupportedExtensions() override;
    std::shared_ptr<Object> Import(AssetDescriptor *asset) override;
};
}