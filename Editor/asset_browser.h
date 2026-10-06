#pragma once
#include "editor_window.h"

namespace editor
{
/// <summary>
/// ImportされたAssetのGui管理をするクラス
/// </summary>
class AssetBrowser : public EditorWindow
{
public:
    std::string Name() override;
    void OnEditorGui() override;
};
}