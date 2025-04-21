#pragma once
#include "engine_editor.h"
#include "../Engine/scene.h"

namespace editor
{
class EditorScene : public engine::Scene
{
public:
    Editor* editor;
    
    void InvokeUpdate() const override;
    void InvokeFixedUpdate() const override;
    void InvokeDraw() const override;
};
}