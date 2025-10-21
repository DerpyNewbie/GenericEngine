#include "pch.h"
#include "bootstrap.h"

#include "application.h"
#include "serializer.h"
#include "engine.h"
#include "scene.h"
#include "scene_manager.h"
#include "default_scene_generator.h"
#include "Asset/asset_database.h"
#include "Editor/editor.h"

namespace
{
std::string SubStrNext(const std::string &src, const std::string &label)
{
    const auto pos = src.find(label);
    if (pos == std::string::npos)
        return "";

    const auto label_end_pos = pos + label.size() + 2;
    const auto first_letter = src.at(label_end_pos - 1);
    if (first_letter == '"' || first_letter == '\'')
    {
        auto end_pos = src.find(first_letter, label_end_pos + 1);
        while (end_pos != std::string::npos && src.at(end_pos - 1) == '\\')
        {
            end_pos = src.find(first_letter, end_pos + 1);
        }

        if (end_pos == std::string::npos)
        {
            engine::Logger::Error("Failed to parse %s: probably not enclosed", label.c_str());
            return "";
        }

        return src.substr(label_end_pos, end_pos - label_end_pos);
    }

    auto next_space = src.find(" ", pos);
    if (next_space == std::string::npos)
        next_space = src.size();

    return src.substr(label_end_pos, next_space - label_end_pos);
}
}

namespace engine
{
bootstrap::LaunchOptions bootstrap::LaunchOptions::Parse(const std::string &args)
{
    LaunchOptions options;
    options.attach_editor = args.find("--no-editor") == std::string::npos;
    options.scene_path = SubStrNext(args, "--scene-path");
    options.resource_path = SubStrNext(args, "--resource-path");
    return options;
}

WPARAM bootstrap::Launch(const LaunchOptions &options)
{
    if (options.attach_editor)
    {
        Application::SetWindowTitle(Application::WindowTitle() + " - Editor");
        editor::Editor::Instance()->Attach();
    }

    if (!options.resource_path.empty())
    {
        Application::SetWindowTitle(Application::WindowTitle() + " - Resource");
        AssetDatabase::SetProjectDirectory(options.resource_path);
    }

    if (!options.scene_path.empty())
    {
        Application::SetWindowTitle(Application::WindowTitle() + " - Scene");
        Serializer serializer;
        std::ifstream ifs(options.scene_path);
        SceneManager::AddScene(serializer.Load<Scene>(ifs));
    }
    else
    {
        Engine::on_default_scene_creation.AddListener(SampleSceneGenerator::CreateDefaultScene);
    }

    return Application::Run();
}
}