#pragma once
#include "material_block.h"
#include "shader.h"
#include "Rendering/CaboRenderingSystem/Material/material_id.h"
#include "Rendering/CaboRenderingSystem/RHI/gpu_resources.h"

namespace engine
{
class Texture2D;
class TextureCube;

/// <summary>
/// Rendererに設定する、Shaderとそのパラメーターの組み合わせです。
/// </summary>
/// <remarks>
/// パラメーターはShaderの変数名で読み書きします。
/// </remarks>
class Material : public Object, public Inspectable
{
    AssetPtr<Shader> m_shader_;

    // CaboRenderingSystemのマテリアル（MaterialRegistryに登録したもの）
    CRS::MaterialId m_renderer_material_;
    // 描画用のマテリアルを作った時のShader。変わったら作り直す
    const Shader *m_renderer_shader_ = nullptr;
    // 描画用のマテリアルに値を反映する必要があるかどうか
    bool m_renderer_dirty_ = true;

    // StructuredBuffer / ByteAddressBuffer のGPU上のバッファ（MaterialBlockの値から作る。保存しない）
    struct GpuBuffer
    {
        CRS::Buffer buffer;
        size_t size = 0;
    };
    std::unordered_map<std::string, GpuBuffer> m_gpu_buffers_;
    // サンプラー（MaterialBlockの設定から作る。保存しない）
    struct GpuSampler
    {
        CRS::Sampler sampler;
        int filter = -1;
        int address = -1;
    };
    std::unordered_map<std::string, GpuSampler> m_gpu_samplers_;
    // 外から渡されたバッファ（SetBuffer(name, handle) / SetGlobalBuffer(name, handle)）。
    // あればMaterialBlockの値の代わりにこれを使う。実行中だけのもので保存しない
    std::unordered_map<std::string, CRS::BufferHandle> m_buffer_overrides_;

    /// <summary>
    /// Shaderに合わせて描画用のマテリアルを作り直します。Shaderが無い場合は標準のShaderを使います。
    /// </summary>
    void CreateRendererMaterial(const std::shared_ptr<Shader> &shader);
    /// <summary>
    /// render_queueとMaterialBlockの値（定数、テクスチャ、バッファ、サンプラー）を描画用のマテリアルに反映します。
    /// </summary>
    void SyncRendererMaterial(const std::shared_ptr<Shader> &shader);
    /// <summary>
    /// バッファのデータをGPUに送ります。大きさが変わった場合は作り直します。
    /// </summary>
    /// <returns>GPU上のバッファ。データが空の場合は無効なハンドル</returns>
    CRS::BufferHandle UploadBuffer(const std::string &name, const void *data, size_t size, size_t stride, bool raw);
    /// <summary>
    /// 設定に合ったサンプラーを返します。設定が変わった場合は作り直します。
    /// </summary>
    CRS::SamplerHandle GetSampler(const std::string &name, int filter, int address);
    /// <summary>
    /// 指定した名前の定数を、型を確かめてから書き込みます。
    /// </summary>
    template <typename T>
    bool SetValue(const std::string &name, const T &value);
    /// <summary>
    /// 指定した名前の定数を読み取ります。無い場合はdefault_valueを返します。
    /// </summary>
    template <typename T>
    T GetValue(const std::string &name, const T &default_value) const;
    /// <summary>
    /// SetGlobal〜で設定された値のうち、このMaterialのShaderに同じ名前・種類のパラメーターがあるものを書き込みます。
    /// </summary>
    void ApplyGlobals();
    /// <summary>
    /// グローバルな値として覚え、今あるすべてのMaterialに書き込みます。
    /// </summary>
    template <typename T>
    static void SetGlobalValue(const std::string &name, const T &value);
    /// <summary>
    /// writerをグローバルな値の書き込み方として覚え、今あるすべてのMaterialのMaterialBlockに対して呼びます。
    /// </summary>
    static void SetGlobalWriter(const std::string &name, std::function<void(MaterialBlock &)> writer);
    /// <summary>
    /// writerをグローバルな値の書き込み方として覚え、今あるすべてのMaterialに対して呼びます（実行中だけの設定用）。
    /// </summary>
    static void SetGlobalMaterialWriter(const std::string &name, std::function<void(Material &)> writer);
    /// <summary>
    /// 外から渡されたバッファを覚えます。無効なハンドルなら外して、MaterialBlockの値に戻します。
    /// </summary>
    void SetBufferOverride(const std::string &name, CRS::BufferHandle buffer);

public:
    // 描く順番のグループ。小さい順に描きます（0..10000）
    uint16_t render_queue = 5000;
    // パラメーターの値。保存・Inspectorでの編集はこれに対して行い、描画の前にCRSのMaterialへ写します
    std::shared_ptr<MaterialBlock> shared_material_block;

    /// <summary>
    /// CRSに登録したMaterialを取り除きます。
    /// </summary>
    ~Material() override;

    /// <summary>
    /// Shaderを指定してMaterialを作ります。
    /// </summary>
    /// <param name="shader_path">ShaderのAssetのパス（例: "Lit.hlsl"）</param>
    /// <param name="name">Materialの名前</param>
    static std::shared_ptr<Material> Create(const std::string &shader_path, const std::string &name = "Material");

    // ---- パラメーター（Shaderの変数名で指定します） ----
    // Set: その名前・型の変数がShaderにない場合はfalseを返します。どのcbufferに入っているかは気にしなくてかまいません
    // Get: その名前の変数がない場合はdefault_valueを返します
    bool SetInt(const std::string &name, int value);
    bool SetFloat(const std::string &name, float value);
    bool SetVector2(const std::string &name, const Vector2 &value);
    bool SetVector3(const std::string &name, const Vector3 &value);
    bool SetColor(const std::string &name, const Color &value);
    bool SetMatrix(const std::string &name, const Matrix &value);
    /// <summary>
    /// Texture2Dを設定します。RenderTexture / DepthTextureも渡せます（別のカメラの描画結果を読む場合）。
    /// 画面のカメラは最後に描かれるので、RenderTextureに描くカメラの結果をそのまま読めます。
    /// </summary>
    bool SetTexture(const std::string &name, const AssetPtr<Texture2D> &texture);
    /// <summary>
    /// TextureCubeを設定します。
    /// </summary>
    bool SetTextureCube(const std::string &name, const AssetPtr<TextureCube> &texture);
    /// <summary>
    /// StructuredBufferの中身を設定します。strideはShaderで宣言した構造体の大きさと同じにしてください。
    /// </summary>
    /// <param name="data">count * stride バイトのデータ</param>
    bool SetBuffer(const std::string &name, const void *data, size_t stride, size_t count);
    /// <summary>
    /// StructuredBufferの中身を、配列から設定します。
    /// </summary>
    template <typename T>
    bool SetBuffer(const std::string &name, std::span<const T> values)
    {
        return SetBuffer(name, values.data(), sizeof(T), values.size());
    }
    /// <summary>
    /// ByteAddressBufferの中身を設定します。GPUには4バイト単位に切り上げて渡します。
    /// </summary>
    bool SetRawBuffer(const std::string &name, const void *data, size_t size);
    /// <summary>
    /// 外で作ったGPUバッファを、値をコピーせずにそのまま使います（StructuredBuffer / ByteAddressBuffer）。
    /// バッファの中身の更新と寿命は呼び出し側が管理します。保存はされません。
    /// 無効なハンドルを渡すと、Materialに保存されている値に戻ります。
    /// バッファはシェーダーの宣言と同じ種類（StructuredならBufferUsage::Structuredと同じstride、ByteAddressならRaw）で作ってください。
    /// </summary>
    bool SetBuffer(const std::string &name, CRS::BufferHandle buffer);
    /// <summary>
    /// サンプラーの補間方法とUVの扱いを設定します。
    /// </summary>
    bool SetSampler(const std::string &name, CRS::Filter filter, CRS::AddressMode address);

    int GetInt(const std::string &name, int default_value = 0) const;
    float GetFloat(const std::string &name, float default_value = 0.0f) const;
    Vector2 GetVector2(const std::string &name, const Vector2 &default_value = {}) const;
    Vector3 GetVector3(const std::string &name, const Vector3 &default_value = {}) const;
    Color GetColor(const std::string &name, const Color &default_value = Color(1, 1, 1, 1)) const;
    Matrix GetMatrix(const std::string &name, const Matrix &default_value = Matrix::Identity) const;
    AssetPtr<Texture2D> GetTexture(const std::string &name) const;
    AssetPtr<TextureCube> GetTextureCube(const std::string &name) const;

    /// <summary>
    /// 指定した名前のパラメーター（定数、テクスチャ、バッファ、サンプラー）があるかどうかを取得します。
    /// </summary>
    bool HasProperty(const std::string &name) const;

    // ---- グローバルな値 ----
    // 今あるすべてのMaterialのうち、Shaderに同じ名前・種類のパラメーターがあるものに書き込みます（無いMaterialは無視します）。
    // 値は覚えておき、後から作ったMaterialやShaderを変えたMaterialにも書き込みます。
    // 書き込んだ値はMaterialの値になるので、Materialを保存するとその値で保存されます。
    // 個別のMaterialにSet〜した値は、次に同じ名前でSetGlobal〜するまで残ります。
    static void SetGlobalInt(const std::string &name, int value);
    static void SetGlobalFloat(const std::string &name, float value);
    static void SetGlobalVector2(const std::string &name, const Vector2 &value);
    static void SetGlobalVector3(const std::string &name, const Vector3 &value);
    static void SetGlobalColor(const std::string &name, const Color &value);
    static void SetGlobalMatrix(const std::string &name, const Matrix &value);
    static void SetGlobalTexture(const std::string &name, const AssetPtr<Texture2D> &texture);
    static void SetGlobalTextureCube(const std::string &name, const AssetPtr<TextureCube> &texture);
    static void SetGlobalBuffer(const std::string &name, const void *data, size_t stride, size_t count);
    template <typename T>
    static void SetGlobalBuffer(const std::string &name, std::span<const T> values)
    {
        SetGlobalBuffer(name, values.data(), sizeof(T), values.size());
    }
    static void SetGlobalRawBuffer(const std::string &name, const void *data, size_t size);
    /// <summary>
    /// 外で作ったGPUバッファを、同じ名前のバッファを持つすべてのMaterialで共有します（値はコピーしません）。
    /// 中身を更新すれば全Materialに反映されます。実行中だけの設定で、保存はされません。
    /// </summary>
    static void SetGlobalBuffer(const std::string &name, CRS::BufferHandle buffer);
    static void SetGlobalSampler(const std::string &name, CRS::Filter filter, CRS::AddressMode address);

    /// <summary>
    /// 描画に使うCaboRenderingSystemのマテリアルを取得します。未作成やShaderが変わった場合は作り、値が変わっていれば反映します。
    /// </summary>
    CRS::MaterialId GetRendererMaterial();

    void OnInspectorGui() override;
    /// <summary>
    /// 標準のShader(Lit.hlsl)を設定します。
    /// </summary>
    void OnConstructed() override;
    /// <summary>
    /// 設定されているShaderのパラメーターから、MaterialBlockを新しく作成します。
    /// </summary>
    void CreateMaterialBlock();

    /// <summary>
    /// 設定されているShaderを取得します。
    /// </summary>
    AssetPtr<Shader> GetShader();

    /// <summary>
    /// Shaderを設定し、MaterialBlockを作り直します。
    /// </summary>
    /// <param name="shader">設定するShader</param>
    void SetShader(const AssetPtr<Shader> &shader);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(shared_material_block)
        );

        if (version >= 3)
        {
            ar(
                CEREAL_NVP(render_queue),
                CEREAL_NVP(m_shader_)
            );
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::Material, 3)
