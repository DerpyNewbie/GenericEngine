#include "pch.h"

#include "RootSignatureManager.h"

RootSignatureManager g_RootSignatureManager;

void RootSignatureManager::Initialize()
{
    RootSignature *root_signature = new RootSignature;
    cache.emplace("Basic", root_signature);
}