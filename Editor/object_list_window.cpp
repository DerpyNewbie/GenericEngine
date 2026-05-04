#include "object_list_window.h"

#include "engine_util.h"
#include "gui.h"

namespace editor
{
std::string ObjectListWindow::Name()
{
    return "Object List";
}
void ObjectListWindow::OnEditorGui()
{
    const auto objects = engine::Object::AllObjects();
    const auto size = objects.size();
    ImGui::Text("Objects: %d", size);
    ImGuiListClipper clipper;
    clipper.Begin(size);
    if (ImGui::BeginTable("OBJECT_LIST", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupColumn("Idx", ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableSetupColumn("Guid", ImGuiTableColumnFlags_WidthFixed, 255);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Destroying", ImGuiTableColumnFlags_WidthFixed, 80);

        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        while (clipper.Step())
        {
            auto it = std::next(objects.begin(), clipper.DisplayStart);
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; ++row_n)
            {
                auto &[guid, object] = *it;
                ImGui::PushID(object.get());
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    if (ImGui::Selectable("##OBJECT_LIST_SELECTION", object == Editor::Instance()->SelectedObject(), ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
                    {
                        Editor::Instance()->SetSelectedObject(object);
                    }

                    engine::Gui::MakeDragDropSource(object);

                    ImGui::SameLine();

                    ImGui::Text("%d", row_n);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", guid.str().c_str());
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", object->Name().c_str());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%s", engine::EngineUtil::GetTypeName(typeid(*object).name()).c_str());
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%s", object->IsDestroying() ? "Yes" : "No");
                }
                ImGui::PopID();

                ++it;
            }
        }

        ImGui::EndTable();
    }
}
}