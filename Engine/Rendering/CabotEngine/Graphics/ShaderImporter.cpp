#include "pch.h"

#include "ShaderImporter.h"

#include "PSOManager.h"

void ShaderImporter::Import()
{}

ShaderObject ShaderImporter::GetDefault()
{
    ShaderObject result;
    result.Name = "default";
    result.VSPath = L"../x64/Debug/BasicVertexShader.cso";
    result.PSPath = L"../x64/Debug/BasicPixelShader.cso";
    result.AddMaterialValues("Value", ShaderObject::ValueType::Float);
    result.AddMaterialValues("Texture1", ShaderObject::ValueType::Texture);

    //FIX:ここでやるべきじゃないかも
    g_PSOManager.Register(result);

    return result;
}