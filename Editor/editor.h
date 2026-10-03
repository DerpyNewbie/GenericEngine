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

    void SetEditorStyle(int i);
    void Init();
    void OnEngineTick() const;

public:
    static std::shared_ptr<Editor> Instance();

    void OnDraw();

    void Attach();
    void Finalize();

    void PushSceneSnapshot();
    engine::Task ApplyLastSceneSnapshot() const;
    engine::Task PopSceneSnapshot();

    void SetEditorMode(EditorMode mode);
    EditorMode GetEditorMode() const;
    void SetPaused(bool is_paused);
    bool IsPaused() const;

    void SingleTickStep();

    void SetSelectedObject(const std::shared_ptr<engine::Object> &object);
    std::shared_ptr<engine::Object> SelectedObject() const;

    void SetSelectedDirectory(const std::filesystem::path &path);
    std::filesystem::path SelectedDirectory() const;

    void AddEditorWindow(const std::string &name, std::shared_ptr<EditorWindow> window);
    std::vector<std::string> GetEditorWindowNames();
    std::shared_ptr<EditorWindow> GetEditorWindow(const std::string &name);
    void RemoveEditorWindow(const std::string &name);

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