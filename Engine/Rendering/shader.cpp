#include "pch.h"
#include "shader.h"

namespace engine
{
std::shared_ptr<Shader> Shader::m_pDefaultShader;

std::shared_ptr<Shader> Shader::GetDefault()
{
    if (!m_pDefaultShader)
    {
        m_pDefaultShader = std::make_shared<Shader>();
        std::wstring file_path = L"x64/Debug/BasicVertexShader.cso";
        auto hr = D3DReadFileToBlob(file_path.c_str(), m_pDefaultShader->m_pVSBlob.GetAddressOf());
        if (FAILED(hr))
        {
            Logger::Error("Failed to create default shader");
        }
        file_path = L"x64/Debug/BasicPixelShader.cso";
        hr = D3DReadFileToBlob(file_path.c_str(), m_pDefaultShader->m_pPSBlob.GetAddressOf());
        if (FAILED(hr))
        {
            Logger::Error("Failed to create default shader");
        }
    }
    return m_pDefaultShader;
}

void Shader::OnInspectorGui()
{
    auto vertex_params = std::views::filter(parameters, [](auto &p) {
        return p->shader_type == kShaderType_Vertex;
    });
    auto pixel_params = std::views::filter(parameters, [](auto &p) {
        return p->shader_type == kShaderType_Pixel;
    });

    auto draw_params = [](auto &params, auto &sources) {
        int index = 0;
        for (auto &param : params)
        {
            ImGui::PushID(&param);
            const auto should_show = ImGui::CollapsingHeader(("Parameter " + std::to_string(index++)).c_str());
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Remove"))
                {
                    sources.erase(std::find_if(sources.begin(), sources.end(), [&](auto &p) {
                        return p->name == param->name && p->index == param->index && p->shader_type == param->
                               shader_type;
                    }));
                    ImGui::EndPopup();
                    ImGui::PopID();
                    break;
                }

                ImGui::EndPopup();
            }

            if (should_show)
            {
                ImGui::Indent();
                ImGui::InputText("Name", &param->name);
                ImGui::InputText("Display Name", &param->display_name);
                ImGui::InputText("Type", &param->type_hint);
                ImGui::Unindent();
            }
            ImGui::PopID();
        }
    };

    ImGui::PushID("Vertex Shader Parameters");
    if (ImGui::CollapsingHeader("Vertex Shader Parameters"))
    {
        ImGui::Indent();
        draw_params(vertex_params, parameters);
        if (ImGui::Button("Add VS Parameter"))
        {
            auto &p = parameters.emplace_back();
            p = std::make_shared<ShaderParameter>();
            p->shader_type = kShaderType_Vertex;
        }
        ImGui::Unindent();
    }
    ImGui::PopID();

    ImGui::PushID("Pixel Shader Parameters");
    if (ImGui::CollapsingHeader("Pixel Shader Parameters"))
    {
        ImGui::Indent();
        draw_params(pixel_params, parameters);
        if (ImGui::Button("Add PS Parameter"))
        {
            auto &p = parameters.emplace_back();
            p = std::make_shared<ShaderParameter>();
            p->shader_type = kShaderType_Pixel;
        }
        ImGui::Unindent();
    }
    ImGui::PopID();
}
}