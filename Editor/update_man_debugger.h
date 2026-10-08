#pragma once
#include "editor_window.h"

namespace editor
{
/// <summary>
/// UpdateManagerの状態を表示するデバッグ用のWindowです。
/// </summary>
class UpdateManDebugger final : public EditorWindow
{
    std::string Name() override;
    /// <summary>
    /// UpdateManagerに登録されているUpdate / FixedUpdateのReceiverの一覧を表示します。
    /// </summary>
    void OnEditorGui() override;
};
}