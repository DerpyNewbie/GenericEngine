#include "pch.h"
#include "default_scene_generator.h"

#include "game_object.h"
#include "Audio/audio_listener_component.h"
#include "Components/camera.h"
#include "Physics/plane_collider.h"
#include "Rendering/model_importer.h"

namespace engine
{
void SampleSceneGenerator::CreateDefaultScene()
{
    CreateDefaultCamera();
    CreateDefaultFloor();
    CreateHackadoll();
}

void SampleSceneGenerator::CreateDefaultCamera()
{
    // Sample scene creation
    const auto camera = Object::Instantiate<GameObject>("Camera");
    camera->AddComponent<Camera>();
    camera->AddComponent<AudioListenerComponent>();

    camera->Transform()->SetLocalPosition({0.0f, 0.85f, 1.5f});
}

void SampleSceneGenerator::CreateDefaultFloor()
{
    Object::Instantiate<GameObject>("Floor")->AddComponent<PlaneCollider>();
}

void SampleSceneGenerator::CreateHackadoll()
{
    auto go = ModelImporter::LoadModelFromFBX("Resources/hackadoll/hackadoll.fbx");
    go->Transform()->SetLocalScale({0.01f, 0.01f, 0.01f});
}
}