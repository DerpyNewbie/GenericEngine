#include "pch.h"
#include "material.h"

#include "MaterialData.h"
#include "gui.h"
#include "Asset/asset_database.h"
#include "Asset/Exporter/asset_exporter.h"
#include "Editor/gui.h"

#include <memory>

void engine::Material::OnInspectorGui()
{
    if (Gui::PropertyField<Shader>("Shader Asset", p_shared_shader))
    {
        if (!p_shared_shader.IsNull())
        {
            CreateMaterialBlock();
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

void engine::Material::OnConstructed()
{
    InspectableAsset::OnConstructed();
    auto default_shader = AssetDatabase::GetAsset(xg::Guid("00214c87-b0e4-40ce-bef6-f7bfb3425888"));
    p_shared_shader = AssetPtr<Shader>::FromIAssetPtr(default_shader);

    CreateMaterialBlock();
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
    m_IsValid = true;
}

bool engine::Material::IsValid()
{
    return m_IsValid;
}

CEREAL_REGISTER_TYPE(engine::Material)