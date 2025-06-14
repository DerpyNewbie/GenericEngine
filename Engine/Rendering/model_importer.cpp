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
    const auto model_handle = MV1LoadModel(file_path);
    if (model_handle == -1)
    {
        Logger::Error<ModelImporter>("Failed to load model from MV1!");
        return nullptr;
    }

    const auto root = Object::Instantiate<GameObject>(RetrieveNameFromPath(file_path));
    const auto root_transform = root->Transform();

    const auto frame_count = MV1GetFrameNum(model_handle);
    auto frame_transforms = std::vector<std::weak_ptr<Transform>>(frame_count);
    auto frame_local_matrix = std::vector<Matrix4x4>(frame_count);

    // Populate frame transforms
    for (int i = 0; i < frame_count; ++i)
    {
        const auto frame_name = EngineUtil::ShiftJisToUtf8(std::string(MV1GetFrameName(model_handle, i)));
        const auto frame_obj = Object::Instantiate<GameObject>(frame_name);
        frame_transforms[i] = frame_obj->Transform();
        frame_local_matrix[i] = MV1GetFrameBaseLocalMatrix(model_handle, i);

        const auto frame_data = frame_obj->AddComponent<FrameMetaData>();
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
                frame_obj->AddComponent<MeshRenderer>()->shared_mesh = mesh;
            }
        }
    }

    // Create parent-child relations
    for (int i = 0; i < frame_count; ++i)
    {
        const auto frame_parent = MV1GetFrameParent(model_handle, i);
        const auto frame_transform = frame_transforms[i].lock();
        frame_transform->SetParent(frame_parent <= -1 ? root_transform : frame_transforms[frame_parent]);
        frame_transform->SetLocalMatrix(frame_local_matrix[i]);
    }

    return root;
}
}