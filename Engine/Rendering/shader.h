#pragma once
#include "shader_parameter.h"
#include "shader_settings.h"
#include "Rendering/CaboRenderingSystem/RHI/gpu_resources.h"

namespace engine
{
class ShaderImporter;

/// <summary>
/// コンパイル済みのShaderです。
/// </summary>
struct CrsShaderProgram
{
    CRS::Shader vertex;
    CRS::Shader pixel;
    CRS::Shader shadow_vertex;
};

/// <summary>
/// .hlslファイルから作るShaderのAssetです。
/// </summary>
/// <remarks>
/// <para>エントリーポイントは VSMain（必須）、PSMain、VSShadow（影用。無い場合は標準の影を使う）です。</para>
/// <para>メッシュ用は Engine/Rendering/CaboRenderingSystem/Shaders/crs_common.hlsli を、
/// UI(Image)用は crs_ui.hlsli をincludeして書きます。</para>
/// <para>Materialのパラメーターは space0 の cbuffer（名前は自由、registerは省略可、b0..b3 まで）の変数と、
/// t0..t7 のリソース（Texture2D、TextureCube、StructuredBuffer、ByteAddressBuffer）、s0..s3 のサンプラーです。
/// ZTestなどの描画設定は "// @Blend Alpha" のように書けます（ShaderSettingsを参照）。</para>
/// </remarks>
class Shader : public Object, public Inspectable
{
    friend ShaderImporter;

    ShaderSettings m_shader_settings_;
    std::shared_ptr<CrsShaderProgram> m_program_;
    // UI(Image)用のShaderかどうか（crs_ui.hlsliをincludeしているか）
    bool m_is_ui_shader_ = false;

    /// <summary>
    /// ZTest、ZWrite、Cull、Blendの設定を編集するGuiを表示します。
    /// </summary>
    void DrawShaderSettings();

public:
    std::vector<ShaderParameter> parameters;

    /// <summary>
    /// .hlslファイルをコンパイルし、描画設定(// @Blend など)とMaterialのパラメーターを読み取ります。
    /// </summary>
    /// <param name="file_path">.hlslファイルのパス</param>
    /// <param name="error_message">失敗時のエラーメッセージの出力先</param>
    /// <returns>コンパイルに成功した場合はtrue</returns>
    bool Compile(const std::wstring &file_path, std::string &error_message);

    /// <summary>
    /// コンパイル済みのShaderを取得します。コンパイルしていない場合はnullptrを返します。
    /// </summary>
    const CrsShaderProgram *CrsProgram() const
    {
        return m_program_.get();
    }

    /// <summary>
    /// UI(Image)用のShaderかどうかを取得します。
    /// </summary>
    bool IsUiShader() const
    {
        return m_is_ui_shader_;
    }

    void OnInspectorGui() override;
    /// <summary>
    /// 描画の設定(ZTest、ZWrite、Cull、Blend)を取得します。
    /// </summary>
    ShaderSettings ShaderSettings() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(parameters),
            CEREAL_NVP(m_shader_settings_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Shader, 1)
