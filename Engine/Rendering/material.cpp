#include "pch.h"

#include "material.h"
#include "gui.h"
#include "Asset/asset_database.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/RootSignature.h"

namespace engine
{
void Material::OnInspectorGui()
{
    if (Gui::PropertyField("RenderQueue", render_queue))
    {
        render_queue = std::clamp(render_queue, static_cast<uint16_t>(0), static_cast<uint16_t>(10000));
    }

    if (Gui::ExpandablePropertyField<Shader>("shader", m_shader_))
    {
        if (m_shader_.CastedLock())
        {
            CreateMaterialBlock();
            return;
        }
    }

    if (ImGui::Button("Reconstruct Material Block"))
    {
        CreateMaterialBlock();
    }

    if (shared_material_block == nullptr)
    {
        ImGui::Text("Material Block is not created.");
    }
    else
    {
        shared_material_block->OnInspectorGui();
    }
}

void Material::OnConstructed()
{
    m_shader_ = AssetDatabase::GetAsset<Shader>("BasicShader.hlsl");
    if (m_shader_.Lock() == nullptr)
    {
        Logger::Warn<Material>("Failed to find Engine Assets");
    }

    CreateMaterialBlock();
}

void Material::CreateMaterialBlock()
{
    if (m_shader_.CastedLock() == nullptr)
    {
        Logger::Error<Material>("Shader is null. Cannot create MaterialBlock.");
        return;
    }

    shared_material_block = std::make_shared<MaterialBlock>();
    shared_material_block->LoadShaderParameters(m_shader_.CastedLock()->parameters);
}

AssetPtr<Shader> Material::GetShader()
{
    return m_shader_;
}

void Material::UpdateBuffer()
{
    if (shared_material_block == nullptr)
    {
        Logger::Log<Material>("MaterialBlock is null. Instantiating!");
        CreateMaterialBlock();
    }
}

bool Material::IsDirty() const
{
    return shared_material_block == nullptr;
}

void Material::SetShader(const AssetPtr<Shader> &shader)
{
    m_shader_ = shader;
    CreateMaterialBlock();
}
}

CEREAL_REGISTER_TYPE(engine::Material)