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

namespace
{
template <class T>
void Reg(const std::string &category)
{
    engine::IComponentFactory::Register(std::make_shared<engine::ComponentFactory<T>>(category));
}
}

namespace engine
{
std::unordered_map<std::string, std::shared_ptr<IComponentFactory>> IComponentFactory::m_factories_;

void IComponentFactory::Init()
{
    Reg<CameraComponent>("Rendering");
    Reg<Controller>("Debug");
    Reg<FrameMetaData>("Debug");
    Reg<MeshRenderer>("Rendering/Renderer");
    Reg<SkinnedMeshRenderer>("Rendering/Renderer");
    Reg<BillboardRenderer>("Rendering/Renderer");
    Reg<TextAssetRefTestComponent>("Debug");
    Reg<TextRenderer>("Rendering/Renderer");
    Reg<Canvas>("Rendering/Renderer");
    Reg<Image>("Rendering/Renderer");
    Reg<RectTransform>("Rendering/Renderer");
    Reg<RigidbodyComponent>("Physics");
    Reg<SphereCollider>("Physics");
    Reg<PlaneCollider>("Physics");
    Reg<BoxCollider>("Physics");
    Reg<CapsuleCollider>("Physics");
    Reg<RigidbodyTesterComponent>("Debug");
    Reg<AudioSourceComponent>("Audio");
    Reg<AudioListenerComponent>("Audio");
    Reg<CinemaCameraComponent>("Rendering/Cinema");
    Reg<CinemaBrainComponent>("Rendering/Cinema");
    Reg<DirectionalLight>("Rendering/Light");
    Reg<SpotLight>("Rendering/Light");
    Reg<RotatorComponent>("Debug");
    Reg<AnimationComponent>("Animation");
    Reg<RenderingSettingsComponent>("Rendering");
    Reg<CinemaCameraTransitioner>("Rendering/Cinema");
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