#pragma once
#include "PipelineState.h"

struct PSOSetting
{
    std::string PSOName;
    std::wstring VSPath;
    std::wstring PSPath;
    D3D12_INPUT_LAYOUT_DESC InputLayout;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveType;
    D3D12_RASTERIZER_DESC RasterizerDesc;
    D3D12_DEPTH_STENCIL_DESC DepthStencilDesc;
    UINT NumRenderTarget;
};

class PSOManager
{
    std::unordered_map<std::string, std::shared_ptr<PipelineState>> m_pso_cache_;

    static std::shared_ptr<PSOManager> m_instance_;

    static std::shared_ptr<PSOManager> Instance();

public:
    static void Initialize();
    static bool Register(PSOSetting setting);
    static bool Register(std::shared_ptr<engine::Shader> shader, std::string pso_name);

    static ID3D12PipelineState *Get(const std::string &id)
    {
        return Instance()->m_pso_cache_[id]->Get();
    }

    static bool SetPipelineState(ID3D12GraphicsCommandList *cmd_list, const std::shared_ptr<engine::Shader> &shader);
};