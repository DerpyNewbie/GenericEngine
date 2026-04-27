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

bool PSOManager::Register(const engine::Shader *shader, const std::string &pso_name, const DXGI_FORMAT rtv_format, const UINT num_render_targets)
{
    PipelineStateSettings pso_setting;
    pso_setting.SetShader(shader);
    pso_setting.layout_desc = engine::Vertex::InputLayout;
    pso_setting.num_rendertarget = num_render_targets;
    pso_setting.rtv_format[0] = rtv_format;

    const auto pso = new PipelineState;
    pso->Create(pso_setting);

    if (!pso->IsValid())
    {
        return false;
    }

    Instance()->m_pso_cache_[pso_name][rtv_format] = std::shared_ptr<PipelineState>(pso);
    return true;
}

bool PSOManager::SetPipelineState(ID3D12GraphicsCommandList *cmd_list, const engine::Shader *shader, const DXGI_FORMAT rtv_format, const UINT num_render_targets)
{
    std::string pso_name = shader->Name();
    auto it = Instance()->m_pso_cache_.find(pso_name);
    if (it == Instance()->m_pso_cache_.end() || !it->second.contains(rtv_format))
    {
        if (!Register(shader, pso_name, rtv_format, num_render_targets))
        {
            engine::Logger::Error<PSOManager>("Failed to register PSO for shader: %s", pso_name);
            return false;
        }
    }

    cmd_list->SetPipelineState(Instance()->m_pso_cache_[pso_name][rtv_format]->Get());
    return true;
}