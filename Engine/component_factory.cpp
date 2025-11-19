#include "pch.h"

#include "component_factory.h"

#include "Animation/animation_component.h"
#include "Audio/audio_listener_component.h"
#include "Audio/audio_source_component.h"
#include "Components/image.h"
#include "Components/text_renderer.h"
#include "Components/billboard_renderer.h"
#include "Components/camera_component.h"
#include "Components/controller.h"
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
#include "Rendering/rendering_settings_component.h"

namespace engine
{
std::unordered_map<std::string, std::shared_ptr<IComponentFactory>> IComponentFactory::m_factories_;

void IComponentFactory::Init()
{
    using namespace component_factory_util;

    Register<CameraComponent>("Rendering");
    Register<Controller>("Debug");
    Register<FrameMetaData>("Debug");
    Register<MeshRenderer>("Rendering/Renderer");
    Register<SkinnedMeshRenderer>("Rendering/Renderer");
    Register<BillboardRenderer>("Rendering/Renderer");
    Register<TextAssetRefTestComponent>("Debug");
    Register<TextRenderer>("Rendering/Renderer");
    Register<Canvas>("Rendering/Renderer");
    Register<Image>("Rendering/Renderer");
    Register<RectTransform>("Rendering/Renderer");
    Register<RigidbodyComponent>("Physics");
    Register<SphereCollider>("Physics");
    Register<PlaneCollider>("Physics");
    Register<BoxCollider>("Physics");
    Register<CapsuleCollider>("Physics");
    Register<RigidbodyTesterComponent>("Debug");
    Register<AudioSourceComponent>("Audio");
    Register<AudioListenerComponent>("Audio");
    Register<CinemaCameraComponent>("Rendering/Cinema");
    Register<CinemaBrainComponent>("Rendering/Cinema");
    Register<DirectionalLight>("Rendering/Light");
    Register<SpotLight>("Rendering/Light");
    Register<RotatorComponent>("Debug");
    Register<AnimationComponent>("Animation");
    Register<RenderingSettingsComponent>("Rendering");
    Register<CinemaCameraTransitioner>("Rendering/Cinema");
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
    return { view.begin(), view.end() };
}
}