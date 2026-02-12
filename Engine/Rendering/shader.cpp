#include "pch.h"
#include "shader.h"
#include "gui.h"

namespace engine
{
std::shared_ptr<Shader> Shader::m_default_shader_;

std::shared_ptr<Shader> Shader::GetDefault()
{
    if (!m_default_shader_)
    {
        m_default_shader_ = std::make_shared<Shader>();
        std::wstring file_path = L"x64/Debug/BasicVertexShader.cso";
        auto hr = D3DReadFileToBlob(file_path.c_str(), m_default_shader_->m_vs_blob_.GetAddressOf());
        if (FAILED(hr))
        {
            Logger::Error("Failed to create default shader");
        }
        file_path = L"x64/Debug/BasicPixelShader.cso";
        hr = D3DReadFileToBlob(file_path.c_str(), m_default_shader_->m_ps_blob_.GetAddressOf());
        if (FAILED(hr))
        {
            Logger::Error("Failed to create default shader");
        }
    }
    return m_default_shader_;
}

void Shader::DrawShaderSettings()
{
    ImGui::Combo("ZTest", &m_shader_settings_.z_test, ZTestNames, IM_ARRAYSIZE(ZTestNames));
    ImGui::Combo("ZWrite", &m_shader_settings_.z_write, ZWriteNames, IM_ARRAYSIZE(ZWriteNames));

    ImGui::Combo("Cull", &m_shader_settings_.cull, CullNames, IM_ARRAYSIZE(CullNames));

    ImGui::Separator();
    ImGui::Text("Blend");
    ImGui::Checkbox("UseBlend", &m_shader_settings_.use_blend);
    ImGui::Combo("SrcFactor", &m_shader_settings_.blend_src, BlendFactorNames, IM_ARRAYSIZE(BlendFactorNames));
    ImGui::Combo("DstFactor", &m_shader_settings_.blend_dst, BlendFactorNames, IM_ARRAYSIZE(BlendFactorNames));
    ImGui::Combo("BlendOp", &m_shader_settings_.blend_op, BlendOpNames, IM_ARRAYSIZE(BlendOpNames));

    ImGui::Combo("ColorMask", &m_shader_settings_.color_mask, ColorMaskNames, IM_ARRAYSIZE(ColorMaskNames));
    ImGui::Checkbox("AlphaToMask(Coverage)", &m_shader_settings_.alpha_to_mask);
}

void Shader::OnInspectorGui()
{
    if (ImGui::CollapsingHeader("ShaderSettings"))
    {
        ImGui::Indent();
        DrawShaderSettings();
        ImGui::Unindent();
    }

    ImGui::PushID("Shader Parameters");
    if (ImGui::CollapsingHeader("Shader Parameters"))
    {
        ImGui::Indent();
        int index = 0;
        for (auto &param : parameters)
        {
            ImGui::PushID(&param);
            const auto should_show = ImGui::CollapsingHeader(("Parameter " + std::to_string(index++)).c_str());

            if (should_show)
            {
                ImGui::Indent();
                Gui::ReadOnlyStringField("Index", std::to_string(param.index));
                Gui::ReadOnlyStringField("Name", param.name);
                ImGui::InputText("Display Name", &param.display_name);
                Gui::ReadOnlyStringField("Type", param.type_hint);
                ImGui::Unindent();
            }
            ImGui::PopID();
        }
    }
    ImGui::PopID();
}

ShaderSettings Shader::ShaderSettings() const
{
    return m_shader_settings_;
}
}