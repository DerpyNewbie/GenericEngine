#include "pch.h"
#include "compute_shader.h"
#include "gui.h"
#include "crs_bridge.h"
#include "CaboRenderingSystem/render_engine.h"

namespace engine
{
namespace
{
std::string ReadText(const std::wstring &file_path)
{
    std::ifstream file(file_path);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

const char *ResourceRegisterPrefix(const CRS::ShaderResourceKind kind)
{
    switch (kind)
    {
    case CRS::ShaderResourceKind::ReadWriteTexture:
    case CRS::ShaderResourceKind::ReadWriteBuffer:
        return "u";
    case CRS::ShaderResourceKind::Sampler:
        return "s";
    default:
        return "t";
    }
}
}

ComputeShader::~ComputeShader()
{
    DestroyPipelineState();
}

void ComputeShader::DestroyPipelineState()
{
    if (m_pipeline_state_.IsValid())
        CrsBridge::Crs().Device().Destroy(m_pipeline_state_);

    m_pipeline_state_ = {};
}

bool ComputeShader::Compile(const std::wstring &file_path, std::string &error_message)
{
    const auto source = ReadText(file_path);
    if (source.empty())
    {
        error_message = "Failed to read compute shader file";
        return false;
    }

    if (source.find("CSMain") == std::string::npos)
    {
        error_message = "Entry point CSMain was not found. Compute shaders must define CSMain.";
        return false;
    }

    auto &device = CrsBridge::Crs().Device();
    CRS::Shader shader(device, {file_path, L"CSMain", CRS::ShaderStage::Compute, {}});
    if (!shader.IsValid())
    {
        error_message = "Failed to compile CSMain (see the log for details)";
        return false;
    }

    CRS::ComputePipelineStateDesc desc;
    desc.compute_shader = shader.GetHandle();
    const auto pipeline_state = device.CreateComputePipelineState(desc);
    if (!pipeline_state.IsValid())
    {
        error_message = "Failed to create compute pipeline state";
        return false;
    }

    DestroyPipelineState();
    m_shader_ = std::move(shader);
    m_pipeline_state_ = pipeline_state;
    return true;
}

void ComputeShader::OnInspectorGui()
{
    if (!IsValid())
    {
        ImGui::TextUnformatted("Not compiled");
        return;
    }

    const auto &reflection = Reflection();
    for (const auto &cb : reflection.constant_buffers)
    {
        Gui::ReadOnlyStringField(("b" + std::to_string(cb.slot) + ", space" + std::to_string(cb.space)).c_str(),
                                 cb.name);
    }

    for (const auto &resource : reflection.resources)
    {
        Gui::ReadOnlyStringField((ResourceRegisterPrefix(resource.kind) + std::to_string(resource.slot) + ", space" +
                                  std::to_string(resource.space)).c_str(), resource.name);
    }
}
}

CEREAL_REGISTER_TYPE(engine::ComputeShader)