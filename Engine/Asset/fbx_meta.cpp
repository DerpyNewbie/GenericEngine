#include "pch.h"
#include "fbx_meta.h"

#include "gui.h"
#include "Components/mesh_renderer.h"
#include "Components/skinned_mesh_renderer.h"

namespace
{
using namespace engine;

void SetupMeshRenderer(const std::shared_ptr<MeshRenderer> &mesh_renderer, const AssetPtr<Mesh> &mesh, const std::shared_ptr<ObjectMeta> &meta)
{
    mesh_renderer->SetSharedMesh(mesh);
    std::ranges::transform(
        meta->mesh.materials,
        std::back_inserter(mesh_renderer->shared_materials),
        [](auto a) {
            return AssetPtr<Material>::FromManaged(a.lock());
        }
        );
}

void PostProcessGameObjects(
    std::map<std::shared_ptr<ObjectMeta>, std::pair<AssetPtr<Mesh>, std::vector<AssetPtr<Material>>>> mesh_objects_map,
    std::map<std::shared_ptr<ObjectMeta>, std::shared_ptr<GameObject>> &conversion_map
    )
{
    for (const auto &[meta, mesh_material_pair] : mesh_objects_map)
    {
        AssetPtr<Mesh> mesh = mesh_material_pair.first;
        std::vector<AssetPtr<Material>> materials = mesh_material_pair.second;

        if (mesh == nullptr)
        {
            Logger::Warn<FbxMeta>("Instantiate: Mesh '%s' not found", meta->name.c_str());
            continue;
        }

        const auto go = conversion_map.at(meta);
        if (!mesh->HasBoneWeights())
        {
            const auto mesh_renderer = go->AddComponent<MeshRenderer>();
            SetupMeshRenderer(mesh_renderer, mesh, meta);
        }
        else
        {
            const auto skinned_mesh_renderer = go->AddComponent<SkinnedMeshRenderer>();
            SetupMeshRenderer(skinned_mesh_renderer, mesh, meta);

            if (auto root_bone_obj_meta = meta->mesh.root_bone.lock())
            {
                const auto root_bone_go = conversion_map.at(root_bone_obj_meta);
                auto root_bone_transform = root_bone_go->Transform();
                skinned_mesh_renderer->root_bone = AssetPtr<Transform>::FromManaged(root_bone_transform);
            }

            std::ranges::transform(
                mesh->bind_poses,
                std::back_inserter(skinned_mesh_renderer->inverted_bind_poses),
                [](auto a) {
                    return a.Invert();
                }
                );

            std::ranges::transform(
                meta->mesh.bones,
                std::back_inserter(skinned_mesh_renderer->transforms),
                [conversion_map](auto a) {
                    return conversion_map.at(a.lock())->Transform();
                }
                );
        }
    }
}

std::shared_ptr<GameObject> CreateGameObjects(
    const std::shared_ptr<GameObject> &parent,
    const std::shared_ptr<ObjectMeta> &meta,
    std::map<std::shared_ptr<ObjectMeta>, std::shared_ptr<GameObject>> &conversion_map
    )
{
    const auto go = GameObject::Instantiate<GameObject>(meta->name);
    conversion_map.emplace(meta, go);

    // Setup GameObject from meta data
    {
        // populate transform
        const auto transform = go->Transform();
        if (parent)
        {
            transform->SetParent(parent->Transform());
        }

        transform->SetLocalMatrix(meta->local_transform.GetMatrix());
    }

    // Create child objects
    for (const auto &child : meta->children)
    {
        CreateGameObjects(go, child, conversion_map);
    }

    return go;
}
}

namespace engine
{
void FbxMeta::OnInspectorGui()
{
    if (ImGui::Button("Instantiate"))
    {
        auto _ = Instantiate();
    }
}

std::shared_ptr<GameObject> FbxMeta::Instantiate() const
{
    std::map<std::shared_ptr<ObjectMeta>, std::shared_ptr<GameObject>> conversion_map;
    auto root_go = CreateGameObjects(nullptr, root_object_meta, conversion_map);
    PostProcessGameObjects(mesh_objects, conversion_map);

    root_go->SetName(Name());
    return root_go;
}
}

CEREAL_REGISTER_TYPE(engine::FbxMeta)