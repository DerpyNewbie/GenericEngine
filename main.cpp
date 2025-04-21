#include "Engine/engine.h"
#include "EngineEditor/engine_editor.h"

#include <DxLib.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    engine::Engine engine;
    editor::Editor editor;
    engine.Init();
    editor.Init();
    editor.Attach(engine);
    engine.MainLoop();
}