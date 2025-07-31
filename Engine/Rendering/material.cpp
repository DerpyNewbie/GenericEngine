#include "pch.h"

#include "material.h"
#include "gui.h"
#include "Asset/asset_database.h"


namespace engine
{
void Material::OnInspectorGui()
{
    if (Gui::ExpandablePropertyField<Shader>("Shader Asset", p_shared_shader))
    {
        if (!p_shared_shader.IsNull())
        {
            CreateMaterialBlock();
            return;
        }
    }

    if (ImGui::Button("Reconstruct Material Block"))
    {
        CreateMaterialBlock();
    }

    if (p_shared_material_block == nullptr)
    {
        ImGui::Text("Material Block is not created.");
    }
    else
    {
        p_shared_material_block->OnInspectorGui();
    }
}

void Material::OnConstructed()
{
    InspectableAsset::OnConstructed();
    auto default_shader = AssetDatabase::GetAsset(xg::Guid("00214c87-b0e4-40ce-bef6-f7bfb3425888"));
    p_shared_shader = AssetPtr<Shader>::FromIAssetPtr(default_shader);
    CreateMaterialBlock();
}

void Material::CreateMaterialBlock()
{
    if (p_shared_material_block)
    {
        p_shared_material_block->DestroyThis();
    }

    const auto shared_shader = p_shared_shader.CastedLock();
    p_shared_material_block = Instantiate<MaterialBlock>("Material Block of " + Name());
    p_shared_material_block->LoadShaderParameters(shared_shader->parameters);
}

void Material::UpdateBuffer()
{
    if (p_shared_material_block == nullptr)
    {
        Logger::Log<Material>("MaterialBlock is null. Instantiating!");
        CreateMaterialBlock();
    }

    p_shared_material_block->UpdateBuffer();
}

bool Material::IsDirty() const
{
    return p_shared_material_block == nullptr || p_shared_material_block->IsDirty();
}

bool Material::IsValid() const
{
    return p_shared_material_block != nullptr;
}
}

CEREAL_REGISTER_TYPE(engine::Material)