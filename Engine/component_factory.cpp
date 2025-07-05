#include "pch.h"
#include "component_factory.h"

#include "Components/camera.h"
#include "Components/controller.h"
#include "Components/cube_renderer.h"
#include "Components/frame_meta_data.h"
#include "Components/mesh_renderer.h"
#include "Components/mv1_renderer.h"
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
    ADD_COMPONENT(CubeRenderer);
    ADD_COMPONENT(FrameMetaData);
    ADD_COMPONENT(MeshRenderer);
    ADD_COMPONENT(MV1Renderer);
    ADD_COMPONENT(SkinnedMeshRenderer);
    ADD_COMPONENT(TextAssetRefTestComponent);
#undef ADD_COMPONENT
}
}