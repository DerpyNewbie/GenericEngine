#include "pch.h"
#include "shader.h"
#include <d3dcompiler.h>
#include <memory>

std::shared_ptr<engine::Shader> engine::Shader::GetDefault()
{
    if (!default_shader)
    {
        default_shader = std::make_shared<Shader>();
        std::wstring file_path = L"x64/Debug/BasicVertexShader.cso";
        auto hr = D3DReadFileToBlob(file_path.c_str(), default_shader->m_pVSBlob.GetAddressOf());
        file_path = L"x64/Debug/BasicPixelShader.cso";
        D3DReadFileToBlob(file_path.c_str(), default_shader->m_pPSBlob.GetAddressOf());
    }
    return default_shader;
}

void engine::Shader::OnInspectorGui()
{
}

void engine::Shader::OnConstructed()
{
    InspectableAsset::OnConstructed();
}