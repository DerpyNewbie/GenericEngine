#include "pch.h"
#include "material.h"

#include "MaterialData.h"
#include "gui.h"
#include "Editor/gui.h"

#include <memory>

void engine::Material::OnInspectorGui()
{
    if (Gui::PropertyField<Shader>("Shader Asset", p_shared_shader))
    {
        if (!p_shared_shader.IsNull())
        {
            CreateMaterialBlock();
            m_IsValid = true;
            return;
        }
        m_IsValid = false;
    }
    if (p_shared_shader.IsLoaded())
    {
        auto shader = p_shared_shader.CastedLock();
        if (!shader)
        {
            Logger::Error<Material>("Invalid reference in material for shader");
        }
        shader->OnInspectorGui();
    }
    if (p_shared_material_block)
    {
        p_shared_material_block->OnInspectorGui();
    }
    else
        ImGui::Text("Not loaded");
}

void engine::Material::CreateMaterialBlock()
{
    if (p_shared_material_block)
    {
        p_shared_material_block->DestroyThis();
    }

    auto shared_shader = p_shared_shader.CastedLock();
    p_shared_material_block = Instantiate<MaterialBlock>();
    p_shared_material_block->CreateParamsFromShaderParams(shared_shader->parameters);
}

bool engine::Material::IsValid()
{
    return m_IsValid;
}

CEREAL_REGISTER_TYPE(engine::Material)