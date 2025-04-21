#pragma once
#include "../Engine/engine.h"

namespace editor
{
class Editor
{
public:
    void Init();
    void Update();
    void Draw();
    void Attach(engine::Engine engine);
};
}