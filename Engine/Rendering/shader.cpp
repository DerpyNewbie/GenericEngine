#include "pch.h"
#include "shader.h"

std::shared_ptr<engine::Shader> engine::Shader::m_pDefaultShader;

std::shared_ptr<engine::Shader> engine::Shader::GetDefault()
{
    if (!m_pDefaultShader)
    {
        m_pDefaultShader = std::make_shared<Shader>();
        std::wstring file_path = L"x64/Debug/BasicVertexShader.cso";
        auto hr = D3DReadFileToBlob(file_path.c_str(), m_pDefaultShader->m_pVSBlob.GetAddressOf());
        if (FAILED(hr))
        {
            Logger::Error("Failed to create default shader");
        }
        file_path = L"x64/Debug/BasicPixelShader.cso";
        hr = D3DReadFileToBlob(file_path.c_str(), m_pDefaultShader->m_pPSBlob.GetAddressOf());
        if (FAILED(hr))
        {
            Logger::Error("Failed to create default shader");
        }
    }
    return m_pDefaultShader;
}

void engine::Shader::OnInspectorGui()
{}

void engine::Shader::OnConstructed()
{
    InspectableAsset::OnConstructed();
}