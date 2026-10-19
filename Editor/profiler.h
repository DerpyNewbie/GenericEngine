#pragma once
#include "editor_window.h"

namespace editor
{
/// <summary>
/// EngineのCycleをStageごとに分け計測されたデータを基にその実行時間をグラフとしてWindowに表示するクラス
/// </summary>
class Profiler final : public EditorWindow
{
public:
    std::string Name() override;
    /// <summary>
    /// 計測されたStageごとの実行時間をグラフとして表示します。
    /// </summary>
    void OnEditorGui() override;
};
}