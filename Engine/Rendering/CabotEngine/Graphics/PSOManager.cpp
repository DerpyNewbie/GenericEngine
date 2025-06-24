#include "pch.h"
#include "PSOManager.h"
#include "RootSignatureManager.h"
#include "SharedStruct.h"
#include "Rendering/Vertex.h"

PSOManager g_PSOManager;

void PSOManager::Initialize()
{
    PSOSetting BasicSetting;
    BasicSetting.PSOName = "Basic";
    BasicSetting.RootSignature = g_RootSignatureManager.Get("Basic");
    BasicSetting.InputLayout = Vertex::InputLayout;
    BasicSetting.VSPath = L"x64/Debug/BasicVertexShader.cso";
    BasicSetting.PSPath = L"x64/Debug/BasicPixelShader.cso";
    Register(BasicSetting);

    //設定の一部が一緒なので使いまわす
    BasicSetting.PSOName = "Line";
    BasicSetting.VSPath = L"x64/Debug/LineVertexShader.cso";
    BasicSetting.PSPath = L"x64/Debug/LinePixelShader.cso";
    Register(BasicSetting);
}

bool PSOManager::Register(PSOSetting setting)
{
    PipelineState *pso = new PipelineState;
    pso->SetInputLayout(setting.InputLayout);
    pso->SetRootSignature(setting.RootSignature);
    pso->SetVS(setting.VSPath);
    pso->SetPS(setting.PSPath);
    pso->Create();

    if (!pso->IsValid())
    {
        return false;
    }

    psoCache.emplace(setting.PSOName, pso);

    return true;
}

bool PSOManager::Register(ShaderObject &shaderObject)
{
    std::shared_ptr<PipelineState> pso = std::make_shared<PipelineState>();
    pso->SetInputLayout(Vertex::InputLayout);
    pso->SetRootSignature(g_RootSignatureManager.Get("Basic"));
    pso->SetVS(shaderObject.VSPath);
    pso->SetPS(shaderObject.PSPath);
    if (!shaderObject.GSPath.empty())
    {
        pso->SetGS(shaderObject.GSPath);
    }
    pso->Create();
    if (!pso->IsValid())
    {
        return false;
    }

    psoCache.emplace(shaderObject.Name, pso);
    return true;
}