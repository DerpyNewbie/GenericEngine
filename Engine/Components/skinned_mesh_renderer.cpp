#include "skinned_mesh_renderer.h"

#include "dxlib_helper.h"
#include "imgui.h"
#include "Rendering/texture2d.h"

void engine::SkinnedMeshRenderer::OnInspectorGui()
{
    ImGui::Checkbox("Draw Bones", &m_draw_bones_);
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Bone Info"))
    {
        for (int i = 0; i < m_bones_.size(); i++)
        {
            const auto &bone_info = m_bones_[i];
            ImGui::Text("%d: %s", bone_info->frame_index, bone_info->name.c_str());
        }
    }

    if (ImGui::CollapsingHeader("Mesh Info"))
    {
        ImGui::Text("Vertices: %d", m_mesh_.vertices.size());
        ImGui::Text("Indices: %d", m_mesh_.indices.size());
        ImGui::Text("UVs: %d", m_mesh_.uv.size());
        ImGui::Text("UV2s: %d", m_mesh_.uv2.size());
        ImGui::Text("Colors: %d", m_mesh_.colors.size());
        ImGui::Text("Normals: %d", m_mesh_.normals.size());
    }

    ImGui::Text("Texture: %d", m_texture_.graphic_handle);
}
void engine::SkinnedMeshRenderer::LoadFromMV1(const std::string &file_path)
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
            MV1GetFrameBaseLocalMatrix(m_model_handle_, i)
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
        m_transforms_[i]->SetLocalMatrix(bone_info->bind_pose);
    }

    m_mesh_ = Mesh::CreateFromMV1(m_model_handle_);
    m_vertex_buffer_handle_ = CreateVertexBuffer(static_cast<int>(m_mesh_.vertices.size()), DX_VERTEX_TYPE_NORMAL_3D);
    if (m_vertex_buffer_handle_ == -1)
    {
        Logger::Error<SkinnedMeshRenderer>("[Load] Failed to create vertex buffer!");
        return;
    }

    m_raw_vertices_.reserve(m_mesh_.vertices.size());
    m_raw_vertices_.resize(m_mesh_.vertices.size());
    for (int i = 0; i < m_mesh_.vertices.size(); i++)
    {
        m_raw_vertices_[i].pos = m_mesh_.vertices[i];
        m_raw_vertices_[i].norm = m_mesh_.normals[i];
        m_raw_vertices_[i].dif = {m_mesh_.colors[i].b, m_mesh_.colors[i].g, m_mesh_.colors[i].r, m_mesh_.colors[i].a};
        m_raw_vertices_[i].u = m_mesh_.uv[i].x;
        m_raw_vertices_[i].v = m_mesh_.uv[i].y;
        m_raw_vertices_[i].su = m_mesh_.uv2[i].x;
        m_raw_vertices_[i].sv = m_mesh_.uv2[i].y;
    }
    if (SetVertexBufferData(0, m_raw_vertices_.data(), static_cast<int>(m_mesh_.vertices.size()),
                            m_vertex_buffer_handle_) == -1)
    {
        Logger::Error<SkinnedMeshRenderer>("[Load] Failed to set vertex buffer data!");
        return;
    }

    m_index_buffer_handle_ = CreateIndexBuffer(static_cast<int>(m_mesh_.indices.size()), DX_INDEX_TYPE_16BIT);
    if (m_index_buffer_handle_ == -1)
    {
        Logger::Error<SkinnedMeshRenderer>("[Load] Failed to create index buffer!");
        return;
    }

    if (SetIndexBufferData(0, m_mesh_.indices.data(), static_cast<int>(m_mesh_.indices.size()),
                           m_index_buffer_handle_) == -1)
    {
        Logger::Error<SkinnedMeshRenderer>("[Load] Failed to set index buffer data!");
        return;
    }

    m_texture_ = Texture2D::GetColoredTexture({0xFF, 0xFF, 0xFF, 0xFF});
}
void engine::SkinnedMeshRenderer::OnDraw()
{
    for (int i = 0; i < m_bones_.size(); i++)
    {
        const auto &bone_info = m_bones_[i];
        MV1SetFrameUserLocalMatrix(m_model_handle_, bone_info->frame_index, m_transforms_[i]->LocalMatrix());
    }

    MV1DrawModel(m_model_handle_);
    DrawPolygonIndexed3D_UseVertexBuffer(m_vertex_buffer_handle_, m_index_buffer_handle_, DX_NONE_GRAPH, false);

    if (m_draw_bones_)
    {
        SetFontSize(12);
        DxLibHelper::DrawModelFrames(m_model_handle_);
    }
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)