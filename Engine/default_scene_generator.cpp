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
}

void SampleSceneGenerator::CreateDefaultCamera()
{
    // Sample scene creation
    const auto camera = Object::Instantiate<GameObject>("Camera");
    camera->AddComponent<CameraComponent>();
    camera->AddComponent<AudioListenerComponent>();
}

void SampleSceneGenerator::CreateDefaultFloor()
{
    Object::Instantiate<GameObject>("Floor")->AddComponent<PlaneCollider>();
}
}