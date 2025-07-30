#include "pch.h"
#include "PSOManager.h"
#include "RootSignatureManager.h"
#include "Rendering/Vertex.h"

PSOManager g_PSOManager;

void PSOManager::Initialize()
{
    PSOSetting BasicSetting;
    BasicSetting.PSOName = "Basic";
    BasicSetting.RootSignature = g_RootSignatureManager.Get("Basic");
    BasicSetting.InputLayout = Vertex::InputLayout;
    BasicSetting.PrimitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    BasicSetting.VSPath = L"x64/Debug/BasicVertexShader.cso";
    BasicSetting.PSPath = L"x64/Debug/BasicPixelShader.cso";
    Register(BasicSetting);

    //設定の一部が一緒なので使いまわす
    BasicSetting.PSOName = "Line";
    BasicSetting.PrimitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    BasicSetting.VSPath = L"x64/Debug/LineVertexShader.cso";
    BasicSetting.PSPath = L"x64/Debug/LinePixelShader.cso";
    Register(BasicSetting);

    BasicSetting.PSOName = "2DBasic";
    BasicSetting.PrimitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    BasicSetting.VSPath = L"x64/Debug/2DVertShader.cso";
    BasicSetting.PSPath = L"x64/Debug/2DPixelShader.cso";
    Register(BasicSetting);
}

bool PSOManager::Register(PSOSetting setting)
{
    PipelineState *pso = new PipelineState;
    pso->SetInputLayout(setting.InputLayout);
    pso->SetRootSignature(setting.RootSignature);
    pso->SetPrimitiveTopologyType(setting.PrimitiveType);
    pso->SetVS(setting.VSPath);
    pso->SetPS(setting.PSPath);
    pso->Create();

    if (!pso->IsValid())
    {
        return false;
    }

    m_PSOCache_.emplace(setting.PSOName, pso);

    return true;
}

bool PSOManager::Register(std::shared_ptr<engine::Shader> shader, std::string pso_name)
{
    auto pso = new PipelineState;
    pso->SetInputLayout(Vertex::InputLayout);
    pso->SetRootSignature(g_RootSignatureManager.Get("Basic"));
    pso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    pso->SetShader(shader);
    pso->Create();

    if (!pso->IsValid())
    {
        return false;
    }

    m_PSOCache_.emplace(pso_name, pso);
    return true;
}

bool PSOManager::SetPipelineState(ID3D12GraphicsCommandList *cmd_list, const std::shared_ptr<engine::Shader> &shader)
{
    //PSO name is the same as the shader name.
    std::string pso_name = shader->Name();
    auto it = m_PSOCache_.find(pso_name);
    if (it != m_PSOCache_.end())
    {
        cmd_list->SetPipelineState(Get(shader->Name()));
        return true;
    }
    return false;
}