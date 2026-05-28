#pragma once
#include "Asset/asset_ptr.h"
#include "Rendering/compute_shader.h"

namespace engine
{
class ComputeShaderTestComponent : public Component
{
    std::shared_ptr<MaterialBlock> m_material_block_;
    AssetPtr<ComputeShader> m_compute_shader_;

    void Execute() const;

public:
    void OnConstructed() override;
    void OnInspectorGui() override;

};
}