#pragma once
#include "event.h"

#include <Windows.h>
#include <functional>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace editor
{
class Editor;
}

class DescriptorHandle;

namespace engine
{
/// <summary>
/// Windowの生成・メッセージループ・プレイモードの状態を管理するアプリケーションクラスです。
/// </summary>
class Application
{
    friend class editor::Editor;

    /// <summary>
    /// Windowクラスを登録し、Windowを生成して表示します。
    /// </summary>
    static void InitWindow();
    /// <summary>
    /// プレイモードの状態を設定します。Editorからのみ呼び出されます。
    /// </summary>
    /// <param name="play">プレイモードにする場合はtrue</param>
    static void SetPlayMode(bool play);

public:
    static Event<> on_window_resized;

    /// <summary>
    /// Applicationの唯一のインスタンスを取得します。
    /// </summary>
    static std::shared_ptr<Application> Instance();

    /// <summary>
    /// Windowとエンジンを初期化し、WM_QUITを受け取るまでメッセージ処理とエンジンのTickを繰り返します。
    /// </summary>
    /// <returns>WM_QUITのwParam(終了コード)</returns>
    static WPARAM Run();

    /// <summary>
    /// Windowメッセージを処理します。リサイズ時にはon_window_resizedを発火し、ImGuiとInputにもメッセージを渡します。
    /// </summary>
    static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    /// <summary>
    /// Windowのクライアント領域の幅を取得します。
    /// </summary>
    static int WindowWidth();
    /// <summary>
    /// Windowのクライアント領域の高さを取得します。
    /// </summary>
    static int WindowHeight();
    /// <summary>
    /// Windowのアスペクト比(幅/高さ)を取得します。
    /// </summary>
    static float WindowAspectRatio();
    /// <summary>
    /// Windowのハンドルを取得します。Window生成前はnullptrです。
    /// </summary>
    static HWND WindowHandle();
    /// <summary>
    /// Windowのタイトルを取得します。
    /// </summary>
    static std::string WindowTitle();
    /// <summary>
    /// Windowのタイトルを設定します。Windowが生成済みであれば即座に反映されます。
    /// </summary>
    /// <param name="title">新しいタイトル</param>
    static void SetWindowTitle(const std::string &title);
    /// <summary>
    /// プレイモード中かどうかを取得します。
    /// </summary>
    static bool IsPlayMode();
};
}