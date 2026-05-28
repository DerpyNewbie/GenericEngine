#pragma once
#include "asset_importer.h"
#include "Rendering/compute_shader.h"

namespace engine
{
class ComputeShaderImporter : public AssetImporter
{

    static bool CompileShader(const std::shared_ptr<ComputeShader> &compute_shader, const std::wstring &file_path, std::string &error_msg);

public:
    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;
};
}