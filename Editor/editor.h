#pragma once
#include "pch.h"

#include "enable_shared_from_base.h"
#include "event_receivers.h"
#include "Coroutine/task.h"

namespace editor
{
class EditorMenu;
class EditorWindow;

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
    struct PrioritizedEditorMenu
    {
        std::string name;
        std::shared_ptr<EditorMenu> menu;
        int priority;
    };

    struct PrioritizedEditorMenuComparator
    {
        bool operator()(const PrioritizedEditorMenu &a, const PrioritizedEditorMenu &b) const
        {
            return a.priority < b.priority;
        }
    };

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
    void Init();
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

    void SetEditorMode(EditorMode mode);
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
    std::shared_ptr<engine::Object> SelectedObject() const;

    /// <summary>
    /// Gui上で選択中のDirectoryを設定します。
    /// </summary>
    /// <param name="path">DirectoryのPath</param>
    void SetSelectedDirectory(const std::filesystem::path &path);
    std::filesystem::path SelectedDirectory() const;

    void AddEditorWindow(const std::string &name, std::shared_ptr<EditorWindow> window);
    void RemoveEditorWindow(const std::string &name);
    
    std::vector<std::string> GetEditorWindowNames();
    std::shared_ptr<EditorWindow> GetEditorWindow(const std::string &name);

    void AddEditorMenu(const std::string &name, const std::shared_ptr<EditorMenu> &menu, int priority = 0);
    std::vector<PrioritizedEditorMenu> GetEditorMenus();
    std::shared_ptr<EditorMenu> GetEditorMenu(const std::string &name);
    void RemoveEditorMenu(const std::string &name);

    void AddCreateMenu(const std::string &name, const std::string &extension,
        std::function<std::shared_ptr<engine::Object>()> factory, int priority = 0);
    std::vector<PrioritizedCreateMenu> GetCreateMenus();
    void RemoveCreateMenu(const std::string &name);

    void DrawEditorMenuBar() const;
    void DrawEditorMenu(const std::string &name);
};
}