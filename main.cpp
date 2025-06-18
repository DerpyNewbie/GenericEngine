#include "pch.h"

#include "Engine/engine.h"
#include "Editor/editor.h"
#include "Engine/scene.h"
#include "Engine/Components/camera.h"
#include "Engine/Components/controller.h"
#include "Engine/Components/skinned_mesh_renderer.h"
#include "Rendering/model_importer.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    const auto engine = std::make_shared<engine::Engine>();
    const auto editor = std::make_shared<editor::Editor>();
    engine->Init();
    editor->Init();
    editor->Attach();

    {
        // Sample scene creation
        const auto camera = engine::Object::Instantiate<engine::GameObject>("Camera");
        camera->AddComponent<engine::Camera>();
        camera->AddComponent<engine::Controller>();

        const auto parent_obj = engine::Object::Instantiate<engine::GameObject>("Parent");
        const auto child_obj = engine::Object::Instantiate<engine::GameObject>("Child");
        child_obj->Transform()->SetParent(parent_obj->Transform());

        const auto parent_obj2 = engine::Object::Instantiate<engine::GameObject>("Parent2");
        const auto child_obj2 = engine::Object::Instantiate<engine::GameObject>("Child2");
        child_obj2->Transform()->SetParent(parent_obj2->Transform());

        engine::ModelImporter::LoadModelFromFBX("Resources/primitives/cube.fbx");
        engine::ModelImporter::LoadModelFromFBX("Resources/primitives/submesh_cube.fbx");
        engine::ModelImporter::LoadModelFromFBX("Resources/hackadoll/hackadoll.fbx");
    }

    engine->MainLoop();

    editor->Finalize();
}