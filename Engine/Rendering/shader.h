#pragma once
#include "object.h"
#include "Asset/inspectable_asset.h"
#include "Asset/Importer/ShaderImporter.h"
#include "CabotEngine/Graphics/ComPtr.h"
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <directx/d3dx12_core.h>

namespace engine
{

/// <summary>
/// Shader object representation
/// </summary>
/// <remarks>
/// Base of all shaders used in the engine. Shaders such as PS, VS inherits from here.
/// </remarks>
class Shader : public InspectableAsset
{
    friend ShaderImporter;
    ComPtr<ID3DBlob> m_pVSBlob;
    ComPtr<ID3DBlob> m_pPSBlob;

public:
    enum class ShaderType
    {
        Vertex,
        Pixel
    };
    
    void OnInspectorGui() override;
    void OnConstructed() override;
    
    CD3DX12_SHADER_BYTECODE GetByteCode(ShaderType type) const
    {
        switch (type)
        {
        case ShaderType::Vertex:
            return m_pVSBlob.Get();
        case ShaderType::Pixel:
            return m_pPSBlob.Get();
        }
    }

};
}