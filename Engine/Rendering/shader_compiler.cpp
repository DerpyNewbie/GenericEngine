#include "pch.h"
#include "shader_compiler.h"

namespace engine
{
bool ShaderCompiler::CompileShader(const std::shared_ptr<Shader> &shader, const std::wstring &file_path, std::string &error_msg)
{
    if (!CompileBlob(shader->m_vs_blob_, file_path, "vrt", "vs_5_0", error_msg))
    {
        error_msg = static_cast<const std::string &>("Failed to Compile Vertex Shader!") + static_cast<const std::string &>("\n") + error_msg;

        return false;
    }

    if (!CompileBlob(shader->m_ps_blob_, file_path, "pix", "ps_5_0", error_msg))
    {
        error_msg = static_cast<const std::string &>("Failed to Compile Pixel Shader!") + static_cast<const std::string &>("\n") + error_msg;

        return false;
    }

    if (!CompileBlob(shader->m_gs_blob_, file_path, "geo", "gs_5_0", error_msg))
    {
        error_msg = static_cast<const std::string &>("Failed to Compile Geometry Shader!") + static_cast<const std::string &>("\n") + error_msg;

        return false;
    }

    // Add shader variants here if you want to
    return true;
}

bool ShaderCompiler::CompileBlob(ComPtr<ID3DBlob> &shader_blob, const std::wstring &file_path, const std::string &entry_point, const std::string &target, std::string &error_msg)
{
    ComPtr<ID3DBlob> error_blob;

    auto hr = D3DCompileFromFile(
        file_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entry_point.c_str(),
        target.c_str(),
        0,
        0,
        &shader_blob,
        &error_blob
    );

    if (FAILED(hr))
    {
        if (error_blob && error_blob
            ->
            GetBufferPointer() && error_blob->GetBufferSize() > 0
        )
        {
            std::string temp_error = static_cast<const char *>(error_blob->GetBufferPointer());

            if (temp_error.find("entrypoint not found") != std::string::npos)
            {
                shader_blob = nullptr;
                return true;
            }

            error_msg = temp_error;
        }
        else
        {
            error_msg = "\0";
        }

        return false;
    }

    return true;
}
}