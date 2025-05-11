#pragma once
#include "hierarchy.h"
#include "inspector.h"
#include "enable_shared_from_base.h"
#include "engine.h"

namespace editor
{
class Editor final : public enable_shared_from_base<Editor>, public IDrawCallReceiver
{
    std::shared_ptr<Hierarchy> m_hierarchy_;
    std::shared_ptr<Inspector> m_inspector_;

public:
    void Init();
    void Update();
    void Attach();
    void OnDraw() override;
    void Finalize();
};
}