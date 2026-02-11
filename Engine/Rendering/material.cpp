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
    auto default_shader = AssetDatabase::GetAsset("BasicShader.hlsl");
    if (default_shader.Lock() == nullptr)
    {
        Logger::Warn<Material>("Failed to find Engine Assets");
    }
    p_shared_shader = AssetPtr<Shader>::FromIAssetPtr(default_shader);

    CreateMaterialBlock();
}

void Material::CreateMaterialBlock()
{
    const auto shared_shader = p_shared_shader.CastedLock();
    if (p_shared_material_block)
    {
        auto material_data = p_shared_material_block->material_data;
        p_shared_material_block->DestroyThis();
        p_shared_material_block = Instantiate<MaterialBlock>("Material Block of " + Name());
        p_shared_material_block->LoadShaderParameters(shared_shader->parameters, material_data);
    }

    if (shared_shader == nullptr)
    {
        Logger::Error<Material>("Shader is null. Cannot create MaterialBlock.");
        return;
    }

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

void Material::SetDescriptorTable()
{
    const auto material_block = p_shared_material_block;
    const auto cmd_list = RenderEngine::CommandList();

    UpdateBuffer();

    for (int shader_i = 0; shader_i < kShaderType_Count; ++shader_i)
    {
        for (int param_i = 0; param_i < kParameterBufferType_Count; ++param_i)
        {
            const auto shader_type = static_cast<kShaderType>(shader_i);
            const auto param_type = static_cast<kParameterBufferType>(param_i);

            if (material_block->Empty(shader_type, param_type))
            {
                continue;
            }

            const int root_param_idx = shader_type * kParameterBufferType_Count + param_i +
                                       RootSignature::kPreDefinedVariableCount;
            const auto itr = material_block->Begin(param_type);
            const auto desc_handle = itr->handle->HandleGPU;
            cmd_list->SetGraphicsRootDescriptorTable(root_param_idx, desc_handle);
        }
    }
}
}

CEREAL_REGISTER_TYPE(engine::Material)