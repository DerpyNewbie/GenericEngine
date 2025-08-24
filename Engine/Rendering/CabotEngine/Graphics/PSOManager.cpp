#include "pch.h"
#include "PSOManager.h"
#include "Rendering/vertex.h"

std::shared_ptr<PSOManager> PSOManager::m_instance_;

std::shared_ptr<PSOManager> PSOManager::Instance()
{
    if (!m_instance_)
    {
        m_instance_ = std::make_shared<PSOManager>();
    }
    return m_instance_;
}

void PSOManager::Initialize()
{
    D3D12_RASTERIZER_DESC rasterizer_desc;

    rasterizer_desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // ラスタライザーはデフォルト
    rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE; // カリングはなし
    rasterizer_desc.FrontCounterClockwise = TRUE;

    D3D12_DEPTH_STENCIL_DESC depth_stencil_desc;

    depth_stencil_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    PSOSetting BasicSetting;
    BasicSetting.PSOName = "Basic";
    BasicSetting.InputLayout = engine::Vertex::InputLayout;
    BasicSetting.PrimitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    BasicSetting.RasterizerDesc = rasterizer_desc;
    BasicSetting.DepthStencilDesc = depth_stencil_desc;
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

    BasicSetting.PSOName = "Skybox";
    BasicSetting.RasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
    BasicSetting.RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    BasicSetting.DepthStencilDesc.DepthEnable = true;
    BasicSetting.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    BasicSetting.DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    BasicSetting.VSPath = L"x64/Debug/SkyboxVertShader.cso";
    BasicSetting.PSPath = L"x64/Debug/SkyboxPixelShader.cso";
    Register(BasicSetting);
}

bool PSOManager::Register(PSOSetting setting)
{
    PipelineState *pso = new PipelineState;
    pso->SetInputLayout(setting.InputLayout);
    pso->SetPrimitiveTopologyType(setting.PrimitiveType);
    pso->SetRasterizerState(setting.RasterizerDesc);
    pso->SetDepthStencilState(setting.DepthStencilDesc);
    pso->SetVS(setting.VSPath);
    pso->SetPS(setting.PSPath);
    pso->Create();

    if (!pso->IsValid())
    {
        return false;
    }

    Instance()->m_PSOCache_.emplace(setting.PSOName, pso);

    return true;
}

bool PSOManager::Register(std::shared_ptr<engine::Shader> shader, std::string pso_name)
{
    auto pso = new PipelineState;
    pso->SetInputLayout(engine::Vertex::InputLayout);
    pso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    pso->SetShader(shader);
    pso->Create();

    if (!pso->IsValid())
    {
        return false;
    }

    Instance()->m_PSOCache_.emplace(pso_name, pso);
    return true;
}

bool PSOManager::SetPipelineState(ID3D12GraphicsCommandList *cmd_list, const std::shared_ptr<engine::Shader> &shader)
{
    //PSO name is the same as the shader name.
    std::string pso_name = shader->Name();
    auto it = Instance()->m_PSOCache_.find(pso_name);
    if (it != Instance()->m_PSOCache_.end())
    {
        cmd_list->SetPipelineState(Get(shader->Name()));
        return true;
    }
    return false;
}