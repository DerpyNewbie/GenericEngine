#pragma once
#include "DxLib.h"
#include "game_object.h"
#include "renderer.h"
#include "Rendering/texture2d.h"
#include "Rendering/mesh.h"

#include <vector>

namespace engine
{
struct BoneInfo
{
    /// @brief Frame index for MV1 models loaded from DxLib
    int frame_index;
    std::string name;
    std::string path;
    Matrix4x4 bind_pose;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(frame_index, name, path);
    }
};

class SkinnedMeshRenderer : public Renderer
{
    int m_model_handle_ = -1;
    bool m_draw_bones_ = false;
    std::vector<std::shared_ptr<BoneInfo>> m_bones_;
    std::vector<std::shared_ptr<Transform>> m_transforms_;
    std::vector<VERTEX3D> m_raw_vertices_;
    Mesh m_mesh_;
    int m_vertex_buffer_handle_ = -1;
    int m_index_buffer_handle_ = -1;
    Texture2D m_texture_;

public:

    void OnInspectorGui() override;
    
    void OnDraw() override;

    void LoadFromMV1(const std::string &file_path);

    template <class Archive>
    void serialize(Archive &ar)
    {
        // TODO: reload model
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_bones_),
           CEREAL_NVP(m_transforms_));
    }
};
}