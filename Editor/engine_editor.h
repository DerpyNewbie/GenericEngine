#pragma once
#include "../Engine/enable_shared_from_base.h"
#include "../Engine/engine.h"

namespace editor
{
class Editor final : public enable_shared_from_base<Editor>, public IDrawCallReceiver
{
public:
    void Init();
    void Update();
    void Attach();
    void OnDraw() override;
};
}