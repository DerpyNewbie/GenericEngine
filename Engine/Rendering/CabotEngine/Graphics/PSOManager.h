#pragma once
#include "PipelineState.h"

/// <summary>
/// ShaderごとのPipelineStateを作成し、キャッシュするクラスです。
/// </summary>
class PSOManager
{
    static std::shared_ptr<PSOManager> m_instance_;
    static std::shared_ptr<PSOManager> Instance();

    using FormatPsoMap = std::unordered_map<DXGI_FORMAT, std::shared_ptr<PipelineState>>;
    std::unordered_map<std::string, FormatPsoMap> m_pso_cache_;

    /// <summary>
    /// ShaderからPipelineStateを作成し、キャッシュに登録します。
    /// </summary>
    /// <param name="shader">使用するShader</param>
    /// <param name="pso_name">キャッシュのキー</param>
    /// <param name="rtv_format">RenderTargetのFormat</param>
    /// <param name="num_render_targets">RenderTargetの数</param>
    /// <returns>作成に失敗した場合 false</returns>
    static bool Register(const engine::Shader *shader, const std::string &pso_name, DXGI_FORMAT rtv_format, UINT num_render_targets);

public:
    /// <summary>
    /// Shaderの名前とRenderTargetのFormatに対応するPipelineStateを、CommandListに設定します。キャッシュにない場合は作成します。
    /// </summary>
    /// <param name="cmd_list">設定先のCommandList</param>
    /// <param name="shader">使用するShader</param>
    /// <param name="rtv_format">RenderTargetのFormat</param>
    /// <param name="num_render_targets">RenderTargetの数</param>
    /// <returns>PipelineStateの作成に失敗した場合 false</returns>
    static bool SetPipelineState(ID3D12GraphicsCommandList *cmd_list, const engine::Shader *shader, DXGI_FORMAT rtv_format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT num_render_targets = 1);
};