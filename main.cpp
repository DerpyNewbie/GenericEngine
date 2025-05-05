#include "Engine/engine.h"
#include "Editor/editor.h"
#include "Engine/camera.h"
#include "Engine/cube_renderer.h"
#include "Engine/scene.h"
#include "Engine/skinned_mesh_renderer.h"

#include <DxLib.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    const auto engine = std::make_shared<engine::Engine>();
    const auto editor = std::make_shared<editor::Editor>();
    engine->Init();
    editor->Init();
    editor->Attach();

    { // Sample scene creation
        const auto camera = engine::Object::Instantiate<engine::GameObject>("Camera");
        camera->AddComponent<engine::Camera>();

        const auto cube_renderer = engine::Object::Instantiate<engine::GameObject>("Cube");
        cube_renderer->AddComponent<engine::CubeRenderer>();

        const auto parent_obj = engine::Object::Instantiate<engine::GameObject>("Parent");
        const auto child_obj = engine::Object::Instantiate<engine::GameObject>("Child");
        child_obj->Transform()->SetParent(parent_obj->Transform());

        const auto parent_obj2 = engine::Object::Instantiate<engine::GameObject>("Parent2");
        const auto child_obj2 = engine::Object::Instantiate<engine::GameObject>("Child2");
        child_obj2->Transform()->SetParent(parent_obj2->Transform());

        const auto skinned_mesh = engine::Object::Instantiate<engine::GameObject>("Hackadoll");
        const auto skinned_mesh_renderer = skinned_mesh->AddComponent<engine::SkinnedMeshRenderer>();
        skinned_mesh_renderer->LoadModel("Resources/hackadoll/hackadoll.pmx");
    }

    engine->MainLoop();

    editor->Finalize();
}