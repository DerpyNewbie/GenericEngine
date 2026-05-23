#include "pch.h"
#include "structured_buffer_data.h"

namespace engine
{
StructuredBufferData::StructuredBufferData(const ShaderParameter &shader_param) : MaterialDataBase(shader_param)
{}

void StructuredBufferData::OnInspectorGui()
{
    if (Gui::PropertyField("Count", m_count_))
        SetCount(m_count_);
}

void StructuredBufferData::SetStride(const size_t stride)
{
    m_stride_ = stride;

    m_data_.resize(m_stride_ * m_count_);
    is_dirty = true;
}

void StructuredBufferData::SetCount(const size_t count)
{
    m_count_ = count;

    m_data_.resize(m_stride_ * m_count_);
    is_dirty = true;
}

void StructuredBufferData::SetData(const void *data)
{
    auto dst = m_data_.data();

    memcpy(dst, data, Size());

    is_dirty = true;
}

void *StructuredBufferData::Data()
{
    return m_data_.data();
}

size_t StructuredBufferData::Stride() const
{
    return m_stride_;
}

size_t StructuredBufferData::Count() const
{
    return m_count_;
}

size_t StructuredBufferData::Size() const
{
    return m_stride_ * m_count_;
}
}

CEREAL_REGISTER_TYPE(engine::StructuredBufferData)