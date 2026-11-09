#pragma once
#include "inspectable.h"
#include "CaboRenderingSystem/RHI/gpu_resources.h"

namespace engine
{
/// <summary>
/// .computeファイルから作るComputeShaderのAssetです。
/// </summary>
/// <remarks>
/// <para>エントリーポイントは CSMain（必須）です。</para>
/// <para>実行は CrsBridge::DispatchCompute で行います。レジスタは b0..b3、t0..t7、u0..u7、s0..s3（すべてspace0）です。</para>
/// </remarks>
class ComputeShader : public Object, public Inspectable
{
    CRS::Shader m_shader_;
    CRS::PipelineStateHandle m_pipeline_state_;

    /// <summary>
    /// 作成済みのPSOを破棄します。
    /// </summary>
    void DestroyPipelineState();

public:
    ~ComputeShader() override;

    /// <summary>
    /// .computeファイルをコンパイルし、PSOを作成します。失敗した場合は以前のコンパイル結果を残します。
    /// </summary>
    /// <param name="file_path">.computeファイルのパス</param>
    /// <param name="error_message">失敗時のエラーメッセージの出力先</param>
    /// <returns>コンパイルに成功した場合はtrue</returns>
    bool Compile(const std::wstring &file_path, std::string &error_message);

    /// <summary>
    /// 実行できる状態（コンパイル済み）かどうかを取得します。
    /// </summary>
    bool IsValid() const
    {
        return m_pipeline_state_.IsValid();
    }

    /// <summary>
    /// コンパイル済みのPSOを取得します。
    /// </summary>
    CRS::PipelineStateHandle PipelineState() const
    {
        return m_pipeline_state_;
    }

    /// <summary>
    /// コンパイル結果のリフレクションを取得します。IsValid()がtrueの場合のみ呼べます。
    /// </summary>
    const CRS::ShaderReflection &Reflection() const
    {
        return m_shader_.Reflection();
    }

    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Object>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::ComputeShader, 1)