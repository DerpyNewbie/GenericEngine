#include "model_importer.h"

#include "game_object.h"

#include <filesystem>

#include "logger.h"
#include "mesh.h"
#include "Components/frame_meta_data.h"
#include "Components/mesh_renderer.h"
#include "Components/skinned_mesh_renderer.h"


namespace engine
{
std::string RetrieveNameFromPath(const char *file_path)
{
    const std::filesystem::path path = file_path;
    return path.stem().string();
}

std::shared_ptr<GameObject> ModelImporter::LoadModelFromMV1(const char *file_path)
{
    auto model_handle = MV1LoadModel(file_path);
    if (model_handle == -1)
    {
        Logger::Error<ModelImporter>("Failed to load model from MV1!");
        return nullptr;
    }

    const auto root = Object::Instantiate<GameObject>(RetrieveNameFromPath(file_path));
    auto root_transform = root->Transform();

    auto bone_count = MV1GetFrameNum(model_handle);
    auto bone_transforms = std::vector<std::weak_ptr<Transform>>(bone_count);
    auto bone_bind_poses = std::vector<Matrix4x4>(bone_count);

    // Populate frame transforms
    for (int i = 0; i < bone_count; ++i)
    {
        const auto bone_name = EngineUtil::ShiftJisToUtf8(std::string(MV1GetFrameName(model_handle, i)));
        const auto bone_obj = Object::Instantiate<GameObject>(bone_name);
        bone_transforms[i] = bone_obj->Transform();
        bone_bind_poses[i] = MV1GetFrameBaseLocalMatrix(model_handle, i);

        const auto frame_data = bone_obj->AddComponent<FrameMetaData>();
        frame_data->max_vert_pos = MV1GetFrameMaxVertexLocalPosition(model_handle, i);
        frame_data->min_vert_pos = MV1GetFrameMinVertexLocalPosition(model_handle, i);
        frame_data->avg_vert_pos = MV1GetFrameAvgVertexLocalPosition(model_handle, i);
        frame_data->bind_pose = MV1GetFrameBaseLocalMatrix(model_handle, i);
        frame_data->verts = MV1GetFrameVertexNum(model_handle, i);
        frame_data->tris = MV1GetFrameTriangleNum(model_handle, i);
        frame_data->meshes = MV1GetFrameMeshNum(model_handle, i);

        if (MV1GetFrameMeshNum(model_handle, i) > 0)
        {
            for (const auto meshes = Mesh::CreateFromMV1(model_handle, i); auto &mesh : meshes)
            {
                bone_obj->AddComponent<MeshRenderer>()->shared_mesh = mesh;
            }
        }
    }

    // Create parent-child relations
    for (int i = 0; i < bone_count; ++i)
    {
        const auto parent = MV1GetFrameParent(model_handle, i);
        const auto bone_transform = bone_transforms[i].lock();
        bone_transform->SetParent(parent <= -1 ? root_transform : bone_transforms[parent]);
        bone_transform->SetLocalMatrix(bone_bind_poses[i]);
    }

    return root;
}
}