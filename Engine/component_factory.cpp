#include "pch.h"

#include "component_factory.h"

#include "Animation/animation_component.h"
#include "Audio/audio_listener_component.h"
#include "Audio/audio_source_component.h"
#include "Components/image.h"
#include "Components/text_renderer.h"
#include "Components/billboard_renderer.h"
#include "Components/camera_component.h"
#include "Components/component_event_tester.h"
#include "Components/controller.h"
#include "Components/coroutine_test.h"
#include "Components/directional_light.h"
#include "Components/frame_meta_data.h"
#include "Components/mesh_renderer.h"
#include "Components/rotator_component.h"
#include "Components/rect_transform.h"
#include "Components/renderer_2d.h"
#include "Components/skinned_mesh_renderer.h"
#include "Components/spot_light.h"
#include "Components/text_asset_ref_test_component.h"
#include "Physics/box_collider.h"
#include "Physics/capsule_collider.h"
#include "Physics/plane_collider.h"
#include "Physics/rigidbody_component.h"
#include "Physics/rigidbody_tester_component.h"
#include "Physics/sphere_collider.h"
#include "Components/Cinema/cinema_brain_component.h"
#include "Components/Cinema/cinema_camera_component.h"
#include "Components/Cinema/cinema_camera_transitioner.h"
#include "Physics/mesh_collider.h"
#include "Rendering/rendering_settings_component.h"

namespace engine
{
std::unordered_map<std::string, std::shared_ptr<IComponentFactory>> IComponentFactory::m_factories_;

void IComponentFactory::Init()
{
    using namespace component_factory_util;

    RegisterComponentFactory<CameraComponent>("Rendering");
    RegisterComponentFactory<Controller>("Debug");
    RegisterComponentFactory<FrameMetaData>("Debug");
    RegisterComponentFactory<ComponentEventTester>("Debug");
    RegisterComponentFactory<MeshRenderer>("Rendering/Renderer");
    RegisterComponentFactory<SkinnedMeshRenderer>("Rendering/Renderer");
    RegisterComponentFactory<BillboardRenderer>("Rendering/Renderer");
    RegisterComponentFactory<TextAssetRefTestComponent>("Debug");
    RegisterComponentFactory<TextRenderer>("Rendering/Renderer");
    RegisterComponentFactory<Canvas>("Rendering/Renderer");
    RegisterComponentFactory<Image>("Rendering/Renderer");
    RegisterComponentFactory<RectTransform>("Rendering/Renderer");
    RegisterComponentFactory<RigidbodyComponent>("Physics");
    RegisterComponentFactory<SphereCollider>("Physics");
    RegisterComponentFactory<PlaneCollider>("Physics");
    RegisterComponentFactory<BoxCollider>("Physics");
    RegisterComponentFactory<CapsuleCollider>("Physics");
    RegisterComponentFactory<MeshCollider>("Physics");
    RegisterComponentFactory<RigidbodyTesterComponent>("Debug");
    RegisterComponentFactory<AudioSourceComponent>("Audio");
    RegisterComponentFactory<AudioListenerComponent>("Audio");
    RegisterComponentFactory<CinemaCameraComponent>("Rendering/Cinema");
    RegisterComponentFactory<CinemaBrainComponent>("Rendering/Cinema");
    RegisterComponentFactory<DirectionalLight>("Rendering/Light");
    RegisterComponentFactory<SpotLight>("Rendering/Light");
    RegisterComponentFactory<RotatorComponent>("Debug");
    RegisterComponentFactory<AnimationComponent>("Animation");
    RegisterComponentFactory<RenderingSettingsComponent>("Rendering");
    RegisterComponentFactory<CinemaCameraTransitioner>("Rendering/Cinema");
    RegisterComponentFactory<CoroutineTest>("Debug");
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