#include "pch.h"
#include "structured_buffer_data.h"

namespace engine
{
StructuredBufferData::StructuredBufferData(const ShaderParameter &shader_param) : BufferDataBase(shader_param)
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
    m_is_size_changed_ = true;
}

void StructuredBufferData::SetCount(const size_t count)
{
    m_count_ = count;

    m_data_.resize(m_stride_ * m_count_);

    is_dirty = true;
    m_is_size_changed_ = true;
}

void StructuredBufferData::SetData(const void *data)
{
    auto dst = m_data_.data();

    memcpy(dst, data, Size());

    is_dirty = true;
}
void StructuredBufferData::SetIsSizeChanged(bool is_size_changed)
{
    m_is_size_changed_ = is_size_changed;
}

bool StructuredBufferData::GetIsSizeChanged() const
{
    return m_is_size_changed_;
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

kBufferType StructuredBufferData::BufferType()
{
    return kBufferType_StructuredBuffer;
}
}

CEREAL_REGISTER_TYPE(engine::StructuredBufferData)