#pragma once
#include <array>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Rendering/CaboRenderingSystem/Core/math_types.h"
#include "Rendering/CaboRenderingSystem/Core/slot_pool.h"
#include "Rendering/CaboRenderingSystem/Material/material_id.h"
#include "Rendering/CaboRenderingSystem/RHI/i_render_device.h"

namespace CRS
{
/// <summary>
/// 1つのパスで使うシェーダーと描画設定です。PSOそのものは持たず、描画時に描画先の設定と合わせて引きます。
/// </summary>
struct MaterialPass
{
    ShaderHandle vertex_shader;
    ShaderHandle pixel_shader;
    RenderState render_state;
};

/// <summary>
/// マテリアル定数の1つの変数の位置です。slotは space0 の b レジスタ番号です。
/// </summary>
struct MaterialProperty
{
    uint32_t slot = 0;
    uint32_t offset = 0;
    uint32_t size = 0;
    ShaderVariableType type = ShaderVariableType::Unknown;
};

/// <summary>
/// 今のパイプライン用のマテリアルです。パスごとのシェーダー、定数、テクスチャを持ちます。
/// 定数とテクスチャは名前で扱い、位置はシェーダーのリフレクションから引きます。
/// space0 の cbuffer は名前に関係なくすべてマテリアルの定数です（b0..b3 まで）。
/// </summary>
class Material
{
    std::array<std::optional<MaterialPass>, static_cast<size_t>(PassTag::Count)> m_passes_;

    std::unordered_map<std::string, MaterialProperty> m_properties_;
    std::unordered_map<std::string, uint32_t> m_texture_slots_;
    std::unordered_map<std::string, uint32_t> m_sampler_slots_;

    // space0 の b レジスタごとの定数
    std::array<std::vector<uint8_t>, kConstantSlotCount> m_constants_;
    std::array<TextureHandle, kResourceSlotCount> m_textures_ = {};
    std::array<SamplerHandle, kSamplerSlotCount> m_samplers_ = {};

    // パスの構成が変わったら進める。PSOのキャッシュの無効化に使う
    uint32_t m_version_ = 0;

    bool Write(const std::string &name, ShaderVariableType type, const void *data, uint32_t size);

public:
    uint16_t render_queue = 2000;

    void SetPass(PassTag tag, const MaterialPass &pass);
    void RemovePass(PassTag tag);
    const MaterialPass *GetPass(PassTag tag) const;

    /// <summary>
    /// メインのパス（Forward）の頂点・ピクセルシェーダーのリフレクションから、プロパティ表を作り直します。
    /// 既に書き込まれている値は、名前と型が同じであれば引き継ぎます。
    /// </summary>
    void BuildPropertyTable(const IRenderDevice &device);

    bool SetInt(const std::string &name, int32_t value);
    bool SetFloat(const std::string &name, float value);
    bool SetVector(const std::string &name, const Vector4 &value);
    bool SetColor(const std::string &name, const Color &value);
    bool SetMatrix(const std::string &name, const Matrix &value);
    /// <summary>
    /// 型を問わず、変数の位置にバイト列を書き込みます。sizeが変数より大きい場合は変数の大きさまで書きます。
    /// </summary>
    bool SetData(const std::string &name, const void *data, uint32_t size);
    bool SetTexture(const std::string &name, TextureHandle texture);
    bool SetSampler(const std::string &name, SamplerHandle sampler);

    bool HasProperty(const std::string &name) const;
    bool HasTexture(const std::string &name) const;

    /// <summary>
    /// 定数を、space0 の b レジスタの位置に設定します。
    /// </summary>
    void BindConstants(Bindings &bindings) const;

    const std::array<TextureHandle, kResourceSlotCount> &Textures() const
    {
        return m_textures_;
    }

    const std::array<SamplerHandle, kSamplerSlotCount> &Samplers() const
    {
        return m_samplers_;
    }

    uint32_t Version() const
    {
        return m_version_;
    }
};

/// <summary>
/// マテリアルの本体を持ちます。メッシュのデータはMaterialIdだけを持ち、ここで引きます。
/// </summary>
class MaterialRegistry
{
    SlotPool<MaterialTag, Material> m_materials_;

public:
    MaterialId Add(Material material);
    void Remove(MaterialId id);
    Material *Get(MaterialId id);
    const Material *Get(MaterialId id) const;
    size_t Count() const;
};
}