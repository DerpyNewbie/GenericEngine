#include "pch.h"
#include "shader.h"
#include "logger.h"

void engine::Shader::OnInspectorGui()
{

}

void engine::Shader::OnConstructed()
{
    InspectableAsset::OnConstructed();
    std::wstring file_path = L"x64/Debug/BasicVertexShader.cso";
    auto hr = D3DReadFileToBlob(file_path.c_str(), m_pVSBlob.GetAddressOf());
    file_path = L"x64/Debug/BasicPixelShader.cso";
    D3DReadFileToBlob(file_path.c_str(), m_pPSBlob.GetAddressOf());
}