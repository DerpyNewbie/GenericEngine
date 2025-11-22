#pragma once

namespace engine::bootstrap
{
struct LaunchOptions
{
    bool attach_editor;
    std::string resource_path;
    std::string scene_path;

    static LaunchOptions Parse(const std::string &args);
};

WPARAM Launch(const LaunchOptions &options);
};