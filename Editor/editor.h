#pragma once
#include "pch.h"

#include "enable_shared_from_base.h"
#include "event_receivers.h"
#include "Coroutine/task.h"

namespace editor
{
class EditorMenu;
class EditorWindow;

/// <summary>
/// Editorの動作モードです。
/// </summary>
enum class EditorMode
{
    kEdit,
    kPlay,
};

/// <summary>
/// Editorの処理のすべてを管理するクラス
/// </summary>
class Editor final : public enable_shared_from_base<Editor>
{
    /// <summary>
    /// 表示順(priority)を持つEditorMenuです。
    /// </summary>
    struct PrioritizedEditorMenu
    {
        std::string name;
        std::shared_ptr<EditorMenu> menu;
        int priority;
    };

    /// <summary>
    /// PrioritizedEditorMenuをpriorityの順に並べるための比較関数です。
    /// </summary>
    struct PrioritizedEditorMenuComparator
    {
        /// <summary>
        /// priorityの昇順で比較します。
        /// </summary>
        bool operator()(const PrioritizedEditorMenu &a, const PrioritizedEditorMenu &b) const
        {
            return a.priority < b.priority;
        }
    };

    /// <summary>
    /// 表示順(priority)を持つ、Assetの作成メニューの項目です。
    /// </summary>
    struct PrioritizedCreateMenu
    {
        std::string name;
        std::string extension;
        std::function<std::shared_ptr<engine::Object>()> factory;
        int priority;
    };

    int m_last_editor_style_ = -1;
    std::weak_ptr<engine::Object> m_selected_object_;
    std::filesystem::path m_selected_directory_ = "";
    std::unordered_map<std::string, std::shared_ptr<EditorWindow>> m_editor_windows_;
    std::vector<PrioritizedEditorMenu> m_editor_menus_;
    std::vector<PrioritizedCreateMenu> m_create_menus_;
    std::queue<std::vector<std::string>> m_scene_snapshots_;

    EditorMode m_mode_ = EditorMode::kEdit;
    bool m_paused_ = false;

    /// <summary>
    /// Editorの見た目を変更する。
    /// </summary>
    /// <param name="i">0 : dark, 1 : Light, 2 : Classic</param>
    void SetEditorStyle(int i);
    /// <summary>
    /// ImGuiの初期化、標準のEditorWindow / EditorMenu / CreateMenuの登録を行います。
    /// </summary>
    void Init();
    /// <summary>
    /// Engineのtick毎に呼ばれます。Pause状態の場合EngineのPlayModeを止めます。
    /// </summary>
    void OnEngineTick() const;

public:
    static std::shared_ptr<Editor> Instance();

    /// <summary>
    /// EditorのDrawを行います。
    /// </summary>
    /// <remark>
    /// Engine側のDrawサイクルの中で呼び出してください。
    /// </remark>
    void OnDraw();

    /// <summary>
    /// EngineにEditorをAttachします。
    /// </summary>
    void Attach();
    /// <summary>
    /// ImGuiの終了処理を行います。
    /// </summary>
    void Finalize();

    /// <summary>
    /// SceneのSnapShotを保存する。
    /// </summary>
    void PushSceneSnapshot();

    /// <summary>
    /// 今あるすべてのSceneを廃棄しSnapShotしてあるSceneに切り替えます。
    /// </summary>
    engine::Task ApplyLastSceneSnapshot() const;

    /// <summary>
    /// SnapShotしてあるSceneに切り替え、そのSceneのSnapShotを削除します。
    /// </summary>
    /// <returns></returns>
    engine::Task PopSceneSnapshot();

    /// <summary>
    /// EditorModeを設定します。kPlayに変更された時SceneのSnapShotを保存し、kEditに戻された時SnapShotしてあるSceneに戻します。
    /// </summary>
    /// <param name="mode">設定するEditorMode</param>
    void SetEditorMode(EditorMode mode);
    /// <summary>
    /// 現在のEditorModeを取得します。
    /// </summary>
    EditorMode GetEditorMode() const;

    /// <summary>
    /// Engineの動作をPauseします。Pause状態の場合EngineのUpdate処理が呼ばれなくなります。
    /// </summary>
    void SetPaused(bool is_paused);

    /// <summary>
    /// Pause状態の場合EngineのUpdate処理が呼ばれなくなります。
    /// </summary>
    /// <returns></returns>
    bool IsPaused() const;

    /// <summary>
    /// Pause状態でない場合Pause状態にし、1フレーム進めます。
    /// </summary>
    void SingleTickStep();

    /// <summary>
    /// Gui上で選択中のObjectを設定します。
    /// </summary>
    /// <param name="object">選択中のオブジェクト</param>
    void SetSelectedObject(const std::shared_ptr<engine::Object> &object);
    /// <summary>
    /// Gui上で選択中のObjectを取得します。
    /// </summary>
    /// <returns>選択中のObject。選択されていない、または既に破棄されている場合 nullptr</returns>
    std::shared_ptr<engine::Object> SelectedObject() const;

    /// <summary>
    /// Gui上で選択中のDirectoryを設定します。
    /// </summary>
    /// <param name="path">DirectoryのPath</param>
    void SetSelectedDirectory(const std::filesystem::path &path);
    /// <summary>
    /// Gui上で選択中のDirectoryを取得します。
    /// </summary>
    /// <returns>DirectoryのPath。選択されていない場合は空</returns>
    std::filesystem::path SelectedDirectory() const;

    /// <summary>
    /// EditorWindowを登録します。
    /// </summary>
    /// <param name="name">Windowの名前</param>
    /// <param name="window">登録するEditorWindow</param>
    void AddEditorWindow(const std::string &name, std::shared_ptr<EditorWindow> window);
    /// <summary>
    /// EditorWindowの登録を解除します。
    /// </summary>
    /// <param name="name">Windowの名前</param>
    void RemoveEditorWindow(const std::string &name);

    /// <summary>
    /// 登録されているすべてのEditorWindowの名前を取得します。
    /// </summary>
    std::vector<std::string> GetEditorWindowNames();
    /// <summary>
    /// 指定された名前のEditorWindowを取得します。登録されていない場合は例外を投げます。
    /// </summary>
    /// <param name="name">Windowの名前</param>
    std::shared_ptr<EditorWindow> GetEditorWindow(const std::string &name);

    /// <summary>
    /// MenuBarにEditorMenuを登録します。
    /// </summary>
    /// <param name="name">Menuの名前</param>
    /// <param name="menu">登録するEditorMenu</param>
    /// <param name="priority">表示の優先度。小さいほど先に表示されます。</param>
    void AddEditorMenu(const std::string &name, const std::shared_ptr<EditorMenu> &menu, int priority = 0);
    /// <summary>
    /// 登録されているすべてのEditorMenuをpriorityの昇順で取得します。
    /// </summary>
    std::vector<PrioritizedEditorMenu> GetEditorMenus();
    /// <summary>
    /// 指定された名前のEditorMenuを取得します。
    /// </summary>
    /// <param name="name">Menuの名前</param>
    std::shared_ptr<EditorMenu> GetEditorMenu(const std::string &name);
    /// <summary>
    /// EditorMenuの登録を解除します。
    /// </summary>
    /// <param name="name">Menuの名前</param>
    void RemoveEditorMenu(const std::string &name);

    /// <summary>
    /// Assetの生成Menuを登録します。
    /// </summary>
    /// <param name="name">Menuの名前</param>
    /// <param name="extension">生成されるAssetの拡張子</param>
    /// <param name="factory">Assetとなるobjectを生成する関数</param>
    /// <param name="priority">表示の優先度。小さいほど先に表示されます。</param>
    void AddCreateMenu(const std::string &name, const std::string &extension,
        std::function<std::shared_ptr<engine::Object>()> factory, int priority = 0);
    /// <summary>
    /// 登録されているすべてのAssetの生成Menuをpriorityの昇順で取得します。
    /// </summary>
    std::vector<PrioritizedCreateMenu> GetCreateMenus();
    /// <summary>
    /// Assetの生成Menuの登録を解除します。
    /// </summary>
    /// <param name="name">Menuの名前</param>
    void RemoveCreateMenu(const std::string &name);

    /// <summary>
    /// 登録されているすべてのEditorMenuをMenuBarに表示します。
    /// </summary>
    void DrawEditorMenuBar() const;
    /// <summary>
    /// 指定された名前のEditorMenuのGuiを表示します。
    /// </summary>
    /// <param name="name">Menuの名前</param>
    void DrawEditorMenu(const std::string &name);
};
}