#include "pch.h"

#include "component_factory.h"

#include "Animation/animation_component.h"
#include "Audio/audio_listener_component.h"
#include "Audio/audio_source_component.h"
#include "Components/Debugging/controller.h"
#include "Components/Debugging/coroutine_test.h"
#include "Components/Debugging/frame_meta_data.h"
#include "Components/Debugging/rotator_component.h"
#include "Components/Debugging/text_asset_ref_test_component.h"
#include "Components/image.h"
#include "Components/text_renderer.h"
#include "Components/billboard_renderer.h"
#include "Components/camera_component.h"
#include "Components/directional_light.h"
#include "Components/mesh_renderer.h"
#include "Components/rect_transform.h"
#include "Components/renderer_2d.h"
#include "Components/skinned_mesh_renderer.h"
#include "Components/spot_light.h"
#include "Physics/box_collider.h"
#include "Physics/capsule_collider.h"
#include "Physics/plane_collider.h"
#include "Physics/rigidbody_component.h"
#include "Physics/rigidbody_tester_component.h"
#include "Physics/sphere_collider.h"
#include "Components/Cinema/cinema_brain_component.h"
#include "Components/Cinema/cinema_camera_component.h"
#include "Components/Cinema/cinema_camera_transitioner.h"
#include "Rendering/rendering_settings_component.h"

namespace engine
{
std::unordered_map<std::string, std::shared_ptr<IComponentFactory>> IComponentFactory::m_factories_;

void IComponentFactory::Init()
{
    using namespace component_factory_util;

    {
        const std::string category_rendering = "Rendering";
        RegisterComponentFactory<CameraComponent>(category_rendering);
        RegisterComponentFactory<RenderingSettingsComponent>(category_rendering);

        {
            const std::string category_renderer = category_rendering + "/Renderer";
            RegisterComponentFactory<MeshRenderer>(category_renderer);
            RegisterComponentFactory<SkinnedMeshRenderer>(category_renderer);
            RegisterComponentFactory<BillboardRenderer>(category_renderer);
            RegisterComponentFactory<TextRenderer>(category_renderer);
            RegisterComponentFactory<Canvas>(category_renderer);
            RegisterComponentFactory<Image>(category_renderer);
            RegisterComponentFactory<RectTransform>(category_renderer);
        }

        {
            const std::string category_cinema = category_rendering + "/Cinema";
            RegisterComponentFactory<CinemaCameraComponent>(category_cinema);
            RegisterComponentFactory<CinemaBrainComponent>(category_cinema);
            RegisterComponentFactory<CinemaCameraTransitioner>(category_cinema);
        }

        {
            const std::string category_light = category_rendering + "/Light";
            RegisterComponentFactory<DirectionalLight>(category_light);
            RegisterComponentFactory<SpotLight>(category_light);
        }
    }

    {
        const std::string category_physics = "Physics";
        RegisterComponentFactory<RigidbodyComponent>(category_physics);
        RegisterComponentFactory<SphereCollider>(category_physics);
        RegisterComponentFactory<PlaneCollider>(category_physics);
        RegisterComponentFactory<BoxCollider>(category_physics);
        RegisterComponentFactory<CapsuleCollider>(category_physics);
    }

    {
        const std::string category_audio = "Audio";
        RegisterComponentFactory<AudioSourceComponent>(category_audio);
        RegisterComponentFactory<AudioListenerComponent>(category_audio);
    }

    {
        const std::string category_animation = "Animation";
        RegisterComponentFactory<AnimationComponent>(category_animation);
    }

    {
        const std::string category_debugging = "Debugging";
        RegisterComponentFactory<Controller>(category_debugging);
        RegisterComponentFactory<FrameMetaData>(category_debugging);
        RegisterComponentFactory<TextAssetRefTestComponent>(category_debugging);
        RegisterComponentFactory<RigidbodyTesterComponent>(category_debugging);
        RegisterComponentFactory<RotatorComponent>(category_debugging);
        RegisterComponentFactory<CoroutineTest>(category_debugging);
    }
}

IComponentFactory::IComponentFactory(const std::string &name, const std::string &friendly_name, const std::string &category) :
    m_name_(name), m_friendly_name_(friendly_name), m_category_(category)
{ }

std::string IComponentFactory::Name()
{
    return m_name_;
}

std::string IComponentFactory::FriendlyName()
{
    return m_friendly_name_;
}

std::string IComponentFactory::Category()
{
    return m_category_;
}

void IComponentFactory::Register(const std::shared_ptr<IComponentFactory> &factory)
{
    m_factories_.insert_or_assign(factory->Name(), factory);
}

std::shared_ptr<IComponentFactory> IComponentFactory::Get(const std::string &name)
{
    return m_factories_.at(name);
}

std::vector<std::string> IComponentFactory::GetNames()
{
    auto view = m_factories_ | std::views::keys;
    return {view.begin(), view.end()};
}
}