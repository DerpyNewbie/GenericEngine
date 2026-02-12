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
        render_queue = std::clamp(render_queue, static_cast<uint64_t>(0), static_cast<uint64_t>(10000));
    }

    if (Gui::ExpandablePropertyField<Shader>("shader", shader))
    {
        if (shader.CastedLock())
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
    shader = AssetDatabase::GetAsset<Shader>("BasicShader.hlsl");
    if (shader.Lock() == nullptr)
    {
        Logger::Warn<Material>("Failed to find Engine Assets");
    }

    CreateMaterialBlock();
}

void Material::CreateMaterialBlock()
{
    if (p_shared_material_block)
    {
        auto material_data = p_shared_material_block->material_data;
        p_shared_material_block->DestroyThis();
        p_shared_material_block = Instantiate<MaterialBlock>("Material Block of " + Name());
        p_shared_material_block->LoadShaderParameters(shader.CastedLock()->parameters, material_data);
    }

    if (shader.CastedLock() == nullptr)
    {
        Logger::Error<Material>("Shader is null. Cannot create MaterialBlock.");
        return;
    }

    p_shared_material_block = Instantiate<MaterialBlock>("Material Block of " + Name());
    p_shared_material_block->LoadShaderParameters(shader.CastedLock()->parameters);
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
            const auto itr = material_block->Begin(shader_type, param_type);
            const auto desc_handle = itr->handle->HandleGPU;
            cmd_list->SetGraphicsRootDescriptorTable(root_param_idx, desc_handle);
        }
    }
}
}

CEREAL_REGISTER_TYPE(engine::Material)