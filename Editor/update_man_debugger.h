#pragma once
#include "editor_window.h"

namespace editor
{
class UpdateManDebugger final : public EditorWindow
{
    std::string Name() override;
    /// <summary>
    /// UpdateManagerに登録されているUpdate / FixedUpdateのReceiverの一覧を表示します。
    /// </summary>
    void OnEditorGui() override;
};
}