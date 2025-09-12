#include "pch.h"
#include "application.h"
#include "default_scene_generator.h"
#include "engine.h"
#include "Editor/editor.h"

using namespace engine;
using namespace editor;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Engine::on_init.AddListener([] {
        Editor::Instance()->Attach();
    });

    Engine::on_default_scene_creation.AddListener([] {
        SampleSceneGenerator::CreateDefaultScene();
    });

    Application::Instance()->StartApp();
}