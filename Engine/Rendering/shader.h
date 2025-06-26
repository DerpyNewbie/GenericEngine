#pragma once
#include "object.h"
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
class Shader : public Object
{
protected:
    ComPtr<ID3DBlob> m_pShaderBlob;
    
public:
    virtual bool CompileShader(std::wstring file_path) = 0;
    CD3DX12_SHADER_BYTECODE GetByteCode() const {return m_pShaderBlob.Get();}
};

class PixelShader : public Shader
{
    bool CompileShader(std::wstring file_path) override;
    
};

class VertexShader : public Shader
{
    bool CompileShader(std::wstring file_path) override;
    
};
}