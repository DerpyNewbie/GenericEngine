#include "pch.h"
#include "material.h"
#include "Editor/gui.h"

void engine::Material::OnInspectorGui()
{
    editor::Gui::PropertyField<Shader>("Shader Asset", p_shared_shader, ".hlsl");
    if (p_shared_shader.IsLoaded())
    {
        std::dynamic_pointer_cast<Shader>(p_shared_shader.Lock())->OnInspectorGui();
        CreateMaterialBlock();
        m_IsValid = true;
    }
    else
        ImGui::Text("Not loaded");
    //Undefined extension
    editor::Gui::PropertyField<MaterialBlock>("MaterialBlock Asset", p_shared_shader, ".matblock");
    if (p_shared_shader.IsLoaded())
        std::dynamic_pointer_cast<MaterialBlock>(p_shared_shader.Lock())->OnInspectorGui();
    else
        ImGui::Text("Not loaded");
}

void engine::Material::CreateMaterialBlock()
{
    auto shared_shader = p_shared_shader.CastedLock();
    auto shared_material_block = p_shared_material_block.CastedLock();
    shared_material_block->CreateParamsFromShaderParams(shared_shader->parameters);
}

bool engine::Material::IsValid()
{
    return m_IsValid;
}

CEREAL_REGISTER_TYPE(engine::Material)