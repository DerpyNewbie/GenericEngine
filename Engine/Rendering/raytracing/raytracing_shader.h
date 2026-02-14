#pragma once

class RaytracingShader
{
    ComPtr<ID3D10Blob> m_shader_blob_;

public:
    CD3DX12_SHADER_BYTECODE GetByteCode() const;

};