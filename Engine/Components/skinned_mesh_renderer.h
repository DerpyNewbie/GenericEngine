#pragma once
#include "DxLib.h"
#include "game_object.h"
#include "renderer.h"

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

public:

    void OnInspectorGui() override;
    
    void OnDraw() override;

    void LoadModel(const std::string &file_path);

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