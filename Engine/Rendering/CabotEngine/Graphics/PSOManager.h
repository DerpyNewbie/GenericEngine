#pragma once
#include "PipelineState.h"

class PSOManager
{
    static std::shared_ptr<PSOManager> m_instance_;
    static std::shared_ptr<PSOManager> Instance();

    using FormatPsoMap = std::unordered_map<DXGI_FORMAT, std::shared_ptr<PipelineState>>;
    std::unordered_map<std::string, FormatPsoMap> m_pso_cache_;

    static bool Register(const engine::Shader *shader, const std::string &pso_name, DXGI_FORMAT rtv_format, UINT num_render_targets);

public:
    static bool SetPipelineState(ID3D12GraphicsCommandList *cmd_list, const engine::Shader *shader, DXGI_FORMAT rtv_format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT num_render_targets = 1);
};