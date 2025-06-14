#include "frame_meta_data.h"

#include "imgui.h"
void engine::FrameMetaData::OnInspectorGui()
{
    ImGui::Text("max: %.2f, %.2f, %.2f", max_vert_pos.x, max_vert_pos.y, max_vert_pos.z);
    ImGui::Text("min: %.2f, %.2f, %.2f", min_vert_pos.x, min_vert_pos.y, min_vert_pos.z);
    ImGui::Text("verts: %d", verts);
    ImGui::Text("tris: %d", tris);
    ImGui::Text("meshes: %d", meshes);
}