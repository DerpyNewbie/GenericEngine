#pragma once
#include <dxcapi.h>

namespace engine
{
class RaytracingShader : public Object
{
    ComPtr<IDxcBlob> m_shader_blob_;

public:
    RaytracingShader(const std::wstring &file_path);
    CD3DX12_SHADER_BYTECODE GetByteCode() const;

};
}