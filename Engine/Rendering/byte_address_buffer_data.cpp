#include "pch.h"
#include "byte_address_buffer_data.h"

engine::ByteAddressBufferData::ByteAddressBufferData(const ShaderParameter &shader_param) : BufferDataBase(shader_param)
{}

void engine::ByteAddressBufferData::OnInspectorGui()
{
    int elem_count = static_cast<int>(m_element_count_);
    if (Gui::PropertyField("Count", elem_count))
        SetCount(static_cast<size_t>(elem_count));
}

void engine::ByteAddressBufferData::SetCount(const size_t count)
{
    m_element_count_ = count;

    m_data_.resize(sizeof(uint8_t) * m_element_count_);

    is_dirty = true;
    is_size_changed = true;
}

void engine::ByteAddressBufferData::SetData(const void *data)
{
    auto dst = m_data_.data();

    memcpy(dst, data, m_element_count_ * sizeof(uint8_t));

    is_dirty = true;
}

void *engine::ByteAddressBufferData::Data()
{
    return m_data_.data();
}

size_t engine::ByteAddressBufferData::Count() const
{
    return m_element_count_;
}

engine::kBufferType engine::ByteAddressBufferData::BufferType()
{
    return kBufferType_ByteAddressBuffer;
}

CEREAL_REGISTER_TYPE(engine::ByteAddressBufferData)