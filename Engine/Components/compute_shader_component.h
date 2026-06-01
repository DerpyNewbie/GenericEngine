#pragma once
#include "Asset/asset_ptr.h"
#include "Rendering/compute_shader.h"

namespace engine
{
class ComputeShaderComponent : public Component
{
    std::shared_ptr<MaterialBlock> m_material_block_;
    AssetPtr<ComputeShader> m_compute_shader_;
    uint32_t m_group_count_x_ = 1;
    uint32_t m_group_count_y_ = 1;
    uint32_t m_group_count_z_ = 1;

    bool m_enable_update_ = false;

    void CreateMaterialBlock();
    
public:
    void OnConstructed() override;
    void OnInspectorGui() override;
    void OnUpdate() override;

    void Execute() const;

    void SetGroupCountX(const uint32_t group_count_x)
    {
        m_group_count_x_ = group_count_x;
    }
    void SetGroupCountY(const uint32_t group_count_y)
    {
        m_group_count_y_ = group_count_y;
    }
    void SetGroupCountZ(const uint32_t group_count_z)
    {
        m_group_count_z_ = group_count_z;
    }

    void SetGroupCount(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);

    void SetBuffer(const std::string &name, const std::shared_ptr<BufferBase> &buffer) const;
    std::shared_ptr<BufferBase> GetBuffer(const std::string &name) const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_compute_shader_),
            CEREAL_NVP(m_material_block_),
            CEREAL_NVP(m_group_count_x_),
            CEREAL_NVP(m_group_count_y_),
            CEREAL_NVP(m_group_count_z_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ComputeShaderComponent, 1)