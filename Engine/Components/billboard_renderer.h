#pragma once
#include "renderer.h"
#include "Rendering/CabotEngine/Graphics/billboard.h"

namespace engine
{
class Material;

class BillboardRenderer : public Renderer
{
    Billboard m_billboard_;

    void SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list);

public:
    std::shared_ptr<Material> shared_material;

    void OnAwake() override;
    void OnConstructed() override;
    void OnInspectorGui() override;
    void OnDraw() override;
    std::shared_ptr<Transform> BoundsOrigin() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this));
    }
};
}