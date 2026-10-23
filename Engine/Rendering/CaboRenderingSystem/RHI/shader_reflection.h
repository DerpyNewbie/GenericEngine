#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace CRS
{
/// <summary>
/// cbufferの変数の型です。Materialの値の書き込みで型を確かめるのに使います。
/// 構造体や配列などここにない型はUnknownになり、名前で値を設定できません。
/// </summary>
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

/// <summary>
/// シェーダーが使うリソース（tレジスタ、uレジスタ、sレジスタ）の種類です。
/// </summary>
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

/// <summary>
/// cbufferの中の1つの変数です。
/// </summary>
struct ShaderVariableInfo
{
    std::string name;
    // cbufferの先頭からのバイト数
    uint32_t offset = 0;
    uint32_t size = 0;
    ShaderVariableType type = ShaderVariableType::Unknown;
};

/// <summary>
/// 1つのcbufferです。cbufferの外に書いた変数は "$Globals" という名前のcbufferにまとめられます。
/// </summary>
struct ConstantBufferInfo
{
    std::string name;
    // bレジスタの番号
    uint32_t slot = 0;
    // レジスタスペース（0: マテリアル、1: エンジン）
    uint32_t space = 0;
    // 全体のバイト数（16バイト単位に切り上げ）
    uint32_t size = 0;
    std::vector<ShaderVariableInfo> variables;
};

/// <summary>
/// 1つのリソース（テクスチャ、バッファ、サンプラー）です。
/// </summary>
struct ShaderResourceInfo
{
    std::string name;
    // t / u / s レジスタの番号
    uint32_t slot = 0;
    // レジスタスペース（0: マテリアル、1: エンジン）
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

    /// <summary>
    /// 指定したレジスタのcbufferを探します。
    /// </summary>
    /// <returns>見つからない場合はnullptr</returns>
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
