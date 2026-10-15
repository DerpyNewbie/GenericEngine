#pragma once
#include "buffer_data_base.h"

namespace engine
{
class ByteAddressBufferData : public BufferDataBase
{
    size_t m_element_count_;
    std::vector<uint8_t> m_data_;

public:
    bool is_size_changed = true;

    ByteAddressBufferData() = default;
    ByteAddressBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    void SetCount(size_t count);
    void SetData(const void *data);

    void *Data();

    [[nodiscard]] size_t Count() const;

    kBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<BufferDataBase>(this),
            CEREAL_NVP(m_element_count_),
            CEREAL_NVP(m_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ByteAddressBufferData, 1)