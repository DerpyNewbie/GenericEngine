#pragma once
#include <string>
#include<unordered_map>
#include "RootSignature.h"

namespace engine
{
class RootSignatureManager
{
public:
    void Initialize();
    ID3D12RootSignature* Get(const std::string& id) { return cache[id]->Get();}

private:
    std::unordered_map<std::string, engine::RootSignature*> cache;
};
}
extern engine::RootSignatureManager g_RootSignatureManager;