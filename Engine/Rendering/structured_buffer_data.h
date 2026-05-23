#pragma once
#include "material_data.h"

namespace engine
{
class StructuredBufferData : public MaterialDataBase
{
    size_t m_stride_ = 4;
    size_t m_count_ = 1;

    std::vector<uint8_t> m_data_;

public:
    StructuredBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    void SetStride(size_t stride);
    void SetCount(size_t count);
    void SetData(const void *data);

    void *Data();

    [[nodiscard]] size_t Stride() const;
    [[nodiscard]] size_t Count() const;
    [[nodiscard]] size_t Size() const;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<MaterialDataBase>(this),
            CEREAL_NVP(m_stride_),
            CEREAL_NVP(m_count_),
            CEREAL_NVP(m_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::StructuredBufferData, 1)