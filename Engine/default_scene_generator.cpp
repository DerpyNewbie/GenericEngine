#include "pch.h"
#include "default_scene_generator.h"

#include "game_object.h"
#include "Audio/audio_listener_component.h"
#include "Components/camera_component.h"
#include "Physics/plane_collider.h"
#include "Rendering/model_importer.h"

namespace engine
{
void SampleSceneGenerator::CreateDefaultScene()
{
    CreateDefaultCamera();
    CreateDefaultFloor();
    CreateYBot();
}

void SampleSceneGenerator::CreateDefaultCamera()
{
    // Sample scene creation
    const auto camera = Object::Instantiate<GameObject>("Camera");
    camera->AddComponent<CameraComponent>();
    camera->AddComponent<AudioListenerComponent>();

    camera->Transform()->SetLocalPosition({0.0f, 0.85f, 1.5f});
}

void SampleSceneGenerator::CreateDefaultFloor()
{
    Object::Instantiate<GameObject>("Floor")->AddComponent<PlaneCollider>();
    const auto floor_cube = ModelImporter::LoadModelFromFBX("Resources/Cube.fbx");
    const auto floor_transform = floor_cube->Transform();
    floor_transform->SetLocalPosition({0, -0.5f, 0});
    floor_transform->SetLocalScale({10, 0.25f, 10});
}

void SampleSceneGenerator::CreateHackadoll()
{
    auto go = ModelImporter::LoadModelFromFBX("Resources/hackadoll/hackadoll.fbx");
    go->Transform()->SetLocalScale({0.01f, 0.01f, 0.01f});
}

void SampleSceneGenerator::CreateYBot()
{
    auto go = ModelImporter::LoadModelFromFBX("Resources/Y Bot.fbx");
    go->Transform()->SetLocalScale({0.01f, 0.01f, 0.01f});
}
}