#include "Engine/engine.h"
#include "Editor/engine_editor.h"

#include <DxLib.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    auto engine = std::make_shared<engine::Engine>();
    auto editor = std::make_shared<editor::Editor>();
    engine->Init();
    editor->Init();
    editor->Attach();
    engine->MainLoop();
}