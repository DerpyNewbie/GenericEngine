#pragma once

#include "PipelineState.h"

struct PSOSetting
{
    std::string PSOName;
    std::wstring VSPath;
    std::wstring PSPath;
    ID3D12RootSignature *RootSignature;
    D3D12_INPUT_LAYOUT_DESC InputLayout;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveType;
};

class PSOManager
{
    std::unordered_map<std::string, std::shared_ptr<PipelineState>> m_PSOCache_;
    std::string m_CurrentPSO_;

public:
    void Initialize();
    bool Register(PSOSetting setting);
    bool Register(std::shared_ptr<engine::Shader> shader);

    ID3D12PipelineState *Get(const std::string &id)
    {
        return m_PSOCache_[id]->Get();
    }

    bool SetPipelineState(ID3D12GraphicsCommandList *cmd_list, const std::shared_ptr<engine::Shader> &shader);

};

extern PSOManager g_PSOManager;