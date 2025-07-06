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
public:
    void Initialize();
    bool Register(PSOSetting setting);

    ID3D12PipelineState *Get(const std::string &id)
    {
        return psoCache[id]->Get();
    }

private:
    std::unordered_map<std::string, std::shared_ptr<PipelineState>> psoCache;
};

extern PSOManager g_PSOManager;