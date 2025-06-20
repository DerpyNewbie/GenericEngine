#pragma once
#include "pch.h"

#include "enable_shared_from_base.h"
#include "event_receivers.h"

namespace editor
{
class EditorWindow;

class Editor final : public enable_shared_from_base<Editor>, public IDrawCallReceiver
{
    int m_last_editor_style_ = -1;
    std::unordered_map<std::string, std::shared_ptr<EditorWindow>> m_editor_windows_;

    void SetEditorStyle(int i);

public:
    int Order() override
    {
        // Editor will get called very last to prevent issues on drawing
        return INT_MAX;
    }

    void Init();
    void Update();
    void Attach();
    void OnDraw() override;
    void Finalize();

    void AddEditorWindow(const std::string &name, std::shared_ptr<EditorWindow> window);
    std::vector<std::string> GetEditorWindowNames();
    std::shared_ptr<EditorWindow> GetEditorWindow(const std::string &name);
    void RemoveEditorWindow(const std::string &name);
};
}