#include "pch.h"

#include "component_factory.h"

#include "Components/text_renderer.h"
#include "Components/billboard_renderer.h"
#include "Components/camera.h"
#include "Components/controller.h"
#include "Components/frame_meta_data.h"
#include "Components/mesh_renderer.h"
#include "Components/skinned_mesh_renderer.h"
#include "Components/text_asset_ref_test_component.h"

namespace engine
{
std::unordered_map<std::string, std::shared_ptr<IComponentFactory>> IComponentFactory::m_factories_;

void IComponentFactory::Init()
{
#define ADD_COMPONENT(type) Register(std::make_shared<ComponentFactory<type>>())
    ADD_COMPONENT(Camera);
    ADD_COMPONENT(Controller);
    ADD_COMPONENT(FrameMetaData);
    ADD_COMPONENT(MeshRenderer);
    ADD_COMPONENT(SkinnedMeshRenderer);
    ADD_COMPONENT(BillboardRenderer);
    ADD_COMPONENT(TextAssetRefTestComponent);
    ADD_COMPONENT(TextRenderer);
#undef ADD_COMPONENT
}

IComponentFactory::IComponentFactory(const std::string &name): m_name_(name)
{
}

std::string IComponentFactory::Name()
{
    return m_name_;
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