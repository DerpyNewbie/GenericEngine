#pragma once
#include "pch.h"

#include "enable_shared_from_base.h"
#include "event_receivers.h"

#include "hierarchy.h"
#include "inspector.h"
#include "enable_shared_from_base.h"
#include "RenderEngine.h"
#include "profiler.h"
#include "update_man_debugger.h"

namespace editor
{
class Editor final : public enable_shared_from_base<Editor>, public IDrawCallReceiver
{
    std::shared_ptr<Hierarchy> m_hierarchy_;
    std::shared_ptr<Inspector> m_inspector_;
    std::shared_ptr<Profiler> m_profiler_;
    std::shared_ptr<UpdateManDebugger> m_update_man_debugger_;

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
};
}