#include "pch.h"

#include "RootSignatureManager.h"

engine::RootSignatureManager g_RootSignatureManager;

void engine::RootSignatureManager::Initialize()
{
    RootSignature *root_signature = new RootSignature;
    cache.emplace("Basic", root_signature);
}