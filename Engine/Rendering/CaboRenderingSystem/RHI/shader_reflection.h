#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace CRS
{
enum class ShaderVariableType : uint8_t
{
    Unknown,
    Int,
    UInt,
    Float,
    Float2,
    Float3,
    Float4,
    Float4x4,
};

enum class ShaderResourceKind : uint8_t
{
    Texture,
    TextureCube,
    StructuredBuffer,
    ByteAddressBuffer,
    ReadWriteTexture,
    ReadWriteBuffer,
    Sampler,
};

struct ShaderVariableInfo
{
    std::string name;
    uint32_t offset = 0;
    uint32_t size = 0;
    ShaderVariableType type = ShaderVariableType::Unknown;
};

struct ConstantBufferInfo
{
    std::string name;
    uint32_t slot = 0;
    uint32_t space = 0;
    uint32_t size = 0;
    std::vector<ShaderVariableInfo> variables;
};

struct ShaderResourceInfo
{
    std::string name;
    uint32_t slot = 0;
    uint32_t space = 0;
    ShaderResourceKind kind = ShaderResourceKind::Texture;
};

/// <summary>
/// コンパイル済みシェーダーのリフレクション結果です。D3D12の型を詰め替えた自前の型です。
/// </summary>
struct ShaderReflection
{
    std::vector<ConstantBufferInfo> constant_buffers;
    std::vector<ShaderResourceInfo> resources;

    const ConstantBufferInfo *FindConstantBuffer(const uint32_t slot, const uint32_t space = 0) const
    {
        for (const auto &cb : constant_buffers)
        {
            if (cb.slot == slot && cb.space == space)
                return &cb;
        }
        return nullptr;
    }
};
}
