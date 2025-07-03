#include "pch.h"
#include "material.h"

std::shared_ptr<engine::Material> engine::Material::CreateFromAiMaterial(const aiScene* scene, int material_idx)
{
    auto result = Instantiate<Material>();
    auto& tex2ds = result->shared_material_block->;

    aiMaterial* mat = scene->mMaterials[material_idx];
    
    for (auto& [key,value] : tex2ds)
    {
        //apply diffuse texture
        if (key == "diffuse")
        {
            aiString tex_path;
            if (scene->mMaterials[material_idx]->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path) == AI_SUCCESS)
            {
                std::string tex_path_s = tex_path.C_Str();
                if (tex_path.C_Str()[0] == '*')
                {
                    tex_path_s.erase(0,1);
                    auto itr = atoi(tex_path_s.c_str());
                    value = std::make_shared<Texture2D>(scene->mTextures[itr]);
                }
                else
                {
                    //implement
                }
            }
            else
            {
                value = Texture2D::GetWhite();
            }
        }
    }
    return result;
}

void engine::Material::OnConstructed()
{
    Object::OnConstructed();
    shared_shader = Instantiate<Shader>();
    shared_material_block = Instantiate<MaterialBlock>();
    
}

void engine::Material::CreateMaterialBlock()
{
    shared_material_block->CreateParamsFromShaderParams(shared_shader->parameters);
}