#pragma once
#include "material_block.h"
#include "shader.h"
#include "Rendering/CaboRenderingSystem/Material/material_id.h"

namespace engine
{
class Texture2D;

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

    /// <summary>
    /// Shaderに合わせて描画用のマテリアルを作り直します。Shaderが無い場合は標準のShaderを使います。
    /// </summary>
    void CreateRendererMaterial(const std::shared_ptr<Shader> &shader);
    /// <summary>
    /// render_queueとMaterialBlockの値（定数、テクスチャ）を描画用のマテリアルに反映します。
    /// </summary>
    void SyncRendererMaterial(const std::shared_ptr<Shader> &shader);
    /// <summary>
    /// 指定した名前の定数を、型を確かめずに書き込みます。
    /// </summary>
    template <typename T>
    bool SetValue(const std::string &name, const T &value);
    /// <summary>
    /// 指定した名前の定数を読み取ります。無い場合はdefault_valueを返します。
    /// </summary>
    template <typename T>
    T GetValue(const std::string &name, const T &default_value) const;

public:
    uint16_t render_queue = 5000;
    std::shared_ptr<MaterialBlock> shared_material_block;

    ~Material() override;

    /// <summary>
    /// Shaderを指定してMaterialを作ります。
    /// </summary>
    /// <param name="shader_path">ShaderのAssetのパス（例: "Lit.hlsl"）</param>
    /// <param name="name">Materialの名前</param>
    static std::shared_ptr<Material> Create(const std::string &shader_path, const std::string &name = "Material");

    // ---- パラメーター（Shaderの変数名で指定します） ----
    bool SetInt(const std::string &name, int value);
    bool SetFloat(const std::string &name, float value);
    bool SetVector2(const std::string &name, const Vector2 &value);
    bool SetVector3(const std::string &name, const Vector3 &value);
    bool SetColor(const std::string &name, const Color &value);
    bool SetMatrix(const std::string &name, const Matrix &value);
    bool SetTexture(const std::string &name, const AssetPtr<Texture2D> &texture);

    int GetInt(const std::string &name, int default_value = 0) const;
    float GetFloat(const std::string &name, float default_value = 0.0f) const;
    Vector2 GetVector2(const std::string &name, const Vector2 &default_value = {}) const;
    Vector3 GetVector3(const std::string &name, const Vector3 &default_value = {}) const;
    Color GetColor(const std::string &name, const Color &default_value = Color(1, 1, 1, 1)) const;
    Matrix GetMatrix(const std::string &name, const Matrix &default_value = Matrix::Identity) const;
    AssetPtr<Texture2D> GetTexture(const std::string &name) const;

    /// <summary>
    /// 指定した名前のパラメーター（定数またはテクスチャ）があるかどうかを取得します。
    /// </summary>
    bool HasProperty(const std::string &name) const;

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
