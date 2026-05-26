#pragma once
#include "buffer_data_base.h"

namespace engine
{
class StructuredBufferData : public BufferDataBase
{
    size_t m_stride_ = 4;
    size_t m_count_ = 1;

    std::vector<uint8_t> m_data_;

public:
    bool is_size_changed = true;

    StructuredBufferData() = default;
    StructuredBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    void SetStride(size_t stride);
    void SetCount(size_t count);
    void SetData(const void *data);

    void *Data();

    [[nodiscard]] size_t Stride() const;
    [[nodiscard]] size_t Count() const;
    [[nodiscard]] size_t Size() const;

    kBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<BufferDataBase>(this),
            CEREAL_NVP(m_stride_),
            CEREAL_NVP(m_count_),
            CEREAL_NVP(m_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::StructuredBufferData, 1)