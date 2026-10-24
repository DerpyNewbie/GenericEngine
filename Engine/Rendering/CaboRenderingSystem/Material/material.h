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
    // 無効なら深度だけを描く（影のパスなど）
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
    // パスごとのシェーダーと描画設定。使わないパスは空
    std::array<std::optional<MaterialPass>, static_cast<size_t>(PassTag::Count)> m_passes_;

    // 名前から、定数の位置 / リソースのtレジスタ / サンプラーのsレジスタを引く表（BuildPropertyTableで作る）
    std::unordered_map<std::string, MaterialProperty> m_properties_;
    std::unordered_map<std::string, uint32_t> m_resource_slots_;
    std::unordered_map<std::string, uint32_t> m_sampler_slots_;
    // tレジスタごとの、シェーダーが宣言しているリソースの種類（宣言されていないスロットは空）
    std::array<std::optional<ShaderResourceKind>, kResourceSlotCount> m_resource_kinds_;

    // space0 の b レジスタごとの定数
    std::array<std::vector<uint8_t>, kConstantSlotCount> m_constants_;
    // space0 の t / s レジスタの番号の位置に入れる。空のスロットはBindResourcesで既定値になる
    std::array<ResourceBinding, kResourceSlotCount> m_resources_ = {};
    std::array<SamplerHandle, kSamplerSlotCount> m_samplers_ = {};

    // パスの構成が変わったら進める。PSOのキャッシュの無効化に使う
    uint32_t m_version_ = 0;

    // 型と大きさを確かめてから、変数の位置に書き込む
    bool Write(const std::string &name, ShaderVariableType type, const void *data, uint32_t size);

public:
    // 描く順番のグループ。小さい順に描く（同じグループの中では、不透明は手前から、半透明は奥から）
    uint16_t render_queue = 2000;

    /// <summary>
    /// パスのシェーダーと描画設定を設定します。プロパティ表は変わらないので、ForwardパスならBuildPropertyTableも呼んでください。
    /// </summary>
    void SetPass(PassTag tag, const MaterialPass &pass);
    /// <summary>
    /// パスを外します。外したパスでは、このマテリアルを使うメッシュは描かれません。
    /// </summary>
    void RemovePass(PassTag tag);
    /// <summary>
    /// パスの設定を取得します。設定されていないパスならnullptrを返します。
    /// </summary>
    const MaterialPass *GetPass(PassTag tag) const;

    /// <summary>
    /// メインのパス（Forward）の頂点・ピクセルシェーダーのリフレクションから、プロパティ表を作り直します。
    /// 既に書き込まれている値は、名前と型が同じであれば引き継ぎます。
    /// </summary>
    void BuildPropertyTable(const IRenderDevice &device);

    // ---- 名前で値を書き込みます。名前がない場合や型が違う場合はfalseを返します ----
    bool SetInt(const std::string &name, int32_t value);
    bool SetFloat(const std::string &name, float value);
    bool SetVector(const std::string &name, const Vector4 &value);
    bool SetColor(const std::string &name, const Color &value);
    bool SetMatrix(const std::string &name, const Matrix &value);
    /// <summary>
    /// 型を問わず、変数の位置にバイト列を書き込みます。sizeが変数より大きい場合は変数の大きさまで書きます。
    /// </summary>
    bool SetData(const std::string &name, const void *data, uint32_t size);
    /// <summary>
    /// 名前のテクスチャ（space0のtレジスタ）を設定します。
    /// </summary>
    bool SetTexture(const std::string &name, TextureHandle texture);
    /// <summary>
    /// 名前のバッファ（StructuredBuffer / ByteAddressBuffer）を設定します。
    /// </summary>
    bool SetBuffer(const std::string &name, BufferHandle buffer);
    /// <summary>
    /// 名前のサンプラー（space0のsレジスタ）を設定します。
    /// </summary>
    bool SetSampler(const std::string &name, SamplerHandle sampler);

    /// <summary>
    /// シェーダーにその名前の定数があるかどうかです。
    /// </summary>
    bool HasProperty(const std::string &name) const;
    /// <summary>
    /// シェーダーにその名前のリソース（テクスチャ、バッファ）があるかどうかです。
    /// </summary>
    bool HasResource(const std::string &name) const;
    /// <summary>
    /// tレジスタのslot番にリソースが設定されているかどうかです。
    /// </summary>
    bool IsResourceSet(uint32_t slot) const;

    /// <summary>
    /// 定数を、space0 の b レジスタの位置に設定します。
    /// </summary>
    void BindConstants(Bindings &bindings) const;
    /// <summary>
    /// リソースを、space0 の t レジスタの位置に設定します。
    /// 設定されていないTexture2Dのスロットと、シェーダーが宣言していないスロットにはdefault_textureを入れます。
    /// それ以外（TextureCube、バッファ）は空のままにします（シェーダーからは0が読めます）。
    /// </summary>
    void BindResources(Bindings &bindings, TextureHandle default_texture) const;

    /// <summary>
    /// s0..s3の位置に並べたサンプラーです。
    /// </summary>
    const std::array<SamplerHandle, kSamplerSlotCount> &Samplers() const
    {
        return m_samplers_;
    }

    /// <summary>
    /// パスの構成が変わるたびに増える番号です。PSOのキャッシュのキーに含めます。
    /// </summary>
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
    /// <summary>
    /// マテリアルを登録します。
    /// </summary>
    MaterialId Add(Material material);
    /// <summary>
    /// マテリアルを取り除きます。このIDを持つメッシュは、そのサブメッシュを描かなくなります。
    /// </summary>
    void Remove(MaterialId id);
    /// <summary>
    /// マテリアルを取得します。無効なIDならnullptrを返します。
    /// </summary>
    Material *Get(MaterialId id);
    const Material *Get(MaterialId id) const;
    /// <summary>
    /// 登録されているマテリアルの数です。
    /// </summary>
    size_t Count() const;
};
}
