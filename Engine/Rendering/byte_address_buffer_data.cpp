#include "pch.h"
#include "byte_address_buffer_data.h"

engine::ByteAddressBufferData::ByteAddressBufferData(const ShaderParameter &shader_param) : BufferDataBase(shader_param)
{}

void engine::ByteAddressBufferData::OnInspectorGui()
{
    if (Gui::PropertyField("Count", m_element_count_))
        SetCount(m_element_count_);
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