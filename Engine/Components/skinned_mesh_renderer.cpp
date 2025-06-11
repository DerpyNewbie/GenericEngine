#include "skinned_mesh_renderer.h"

void engine::SkinnedMeshRenderer::LoadModel(const std::string &file_path)
{
    m_model_handle_ = MV1LoadModel(file_path.c_str());
    const auto frame_num = MV1GetFrameNum(m_model_handle_);
    m_bones_.reserve(frame_num);
    m_transforms_.reserve(frame_num);

    if (frame_num <= 0)
        return;

    // load pass
    for (int i = 0; i < frame_num; i++)
    {
        auto name = EngineUtil::ShiftJisToUtf8(std::string(MV1GetFrameName(m_model_handle_, i)));
        auto info = std::make_shared<BoneInfo>(
            i,
            name,
            "##UNINITIALIZED",
            MV1GetFrameLocalMatrix(m_model_handle_, i)
            );

        m_bones_.emplace_back(info);
        m_transforms_.emplace_back(Instantiate<engine::GameObject>(info->name)->Transform());
    }

    const auto this_transform = GameObject()->Transform();
    // parenting pass
    for (int i = 0; i < frame_num; i++)
    {
        const auto parent = MV1GetFrameParent(m_model_handle_, i);
        if (parent <= -1)
        {
            m_transforms_[i]->SetParent(this_transform);
            continue;
        }

        m_transforms_[i]->SetParent(m_transforms_[parent]);
    }

    // apply matrix pass
    for (int i = 0; i < frame_num; i++)
    {
        const auto bone_info = m_bones_[i];
        bone_info->path = GameObject()->PathFrom(GameObject());
        m_transforms_[i]->SetLocalMatrix(bone_info->rest_pose);
    }
}
void engine::SkinnedMeshRenderer::OnDraw()
{
    
    MV1DrawModel(m_model_handle_);
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)