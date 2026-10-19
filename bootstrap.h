#pragma once

namespace engine::bootstrap
{
/// <summary>
/// 起動引数から読み取った起動設定です。
/// </summary>
struct LaunchOptions
{
    bool attach_editor;
    std::string resource_path;
    std::string scene_path;

    /// <summary>
    /// コマンドライン引数を解析してLaunchOptionsを生成します。--no-editor、--scene-path、--resource-pathに対応しています。
    /// </summary>
    /// <param name="args">コマンドライン引数の文字列</param>
    static LaunchOptions Parse(const std::string &args);
};

/// <summary>
/// 起動設定に従ってEditorの接続、リソースパスと初期Sceneの設定を行い、アプリケーションを実行します。
/// </summary>
/// <param name="options">起動設定</param>
/// <returns>アプリケーションの終了コード</returns>
WPARAM Launch(const LaunchOptions &options);
};