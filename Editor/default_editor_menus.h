#pragma once
#include "editor_menu.h"

namespace editor
{
/// <summary>
/// MenuBarのGuiを管理するクラス
/// </summary>
class DefaultEditorMenu final : public EditorMenu
{
public:
    /// <summary>
    /// Menuの名前に対応するMenuのGuiを表示します。対応するMenuがない場合は例外を投げます。
    /// </summary>
    /// <param name="name">Menuの名前</param>
    void OnEditorMenuGui(std::string name) override;

    /// <summary>
    /// 基本的なMenuのGuiを表示する。
    /// </summary>
    static void DrawDefaultMenu();

    /// <summary>
    /// SceneのLoad、Saveに関するGuiを表示する。
    /// </summary>
    static void DrawFilesMenu();

    /// <summary>
    /// Editorに関するGuiを表示する。
    /// </summary>
    static void DrawEditMenu();

    /// <summary>
    /// WindowのリストのGuiを表示する
    /// </summary>
    static void DrawWindowMenu();

    /// <summary>
    /// Assetの生成のGuiを表示する
    /// </summary>
    /// <param name="path">生成先のファイルパス</param>
    static bool DrawAssetMenu(const std::filesystem::path &path);
};
}