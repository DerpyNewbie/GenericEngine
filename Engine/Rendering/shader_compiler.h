#pragma once
#include "shader.h"

namespace engine
{
class ShaderCompiler
{
    static bool CompileBlob(ComPtr<ID3DBlob> &shader_blob, const std::wstring &file_path, const std::string &entry_point, const std::string &target, std::string &error_msg);

public:
    static bool CompileShader(const std::shared_ptr<Shader> &shader, const std::wstring &file_path, std::string &error_msg);

};
}