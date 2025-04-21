#include "editor_scene.h"
void editor::EditorScene::InvokeUpdate() const
{
    editor->Update();
    
}
void editor::EditorScene::InvokeFixedUpdate() const
{}
void editor::EditorScene::InvokeDraw() const
{
    editor->Draw();
}