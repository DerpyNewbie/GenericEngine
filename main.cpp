#include "pch.h"
#include "application.h"
#include "default_scene_generator.h"
#include "engine.h"
#include "Editor/editor.h"

using namespace engine;
using namespace editor;

int WINAPI WinMain(HINSTANCE, HINSTANCE, const LPSTR lpCmdLine, int)
{
    auto args = std::string(lpCmdLine);

    if (args.find("-no-editor") == std::string::npos && args.find("--ne") == std::string::npos)
    {
        Editor::Instance()->Attach();
    }

    if (args.find("-no-default-scene") == std::string::npos && args.find("--ns") == std::string::npos)
    {
        Engine::on_default_scene_creation.AddListener(SampleSceneGenerator::CreateDefaultScene);
    }

    Application::Instance()->StartApp();
}