#pragma once
#include "editor_window.h"

namespace editor
{
/// <summary>
/// Engine側でのAudioの設定などに関するGuiを管理するクラス
/// </summary>
class AudioWindow : public EditorWindow
{
public:
    std::string Name() override;
    void OnEditorGui() override;
};
}