#pragma once
#include "gui.h"
#include "ibuffer.h"
#include "allowed_buffer_type.h"
#include "../CabotEngine/Graphics/ConstantBuffer.h"

namespace engine
{
template <typename T> requires kAllowedBufferType<T>
class ConstantBufferAsset : public IBuffer
{
    std::unique_ptr<ConstantBuffer> m_buffer_ = nullptr;
    bool m_is_dirty_ = true;
    T m_data_;

public:
    void OnInspectorGui() override;
    bool CreateBuffer() override;
    void SetValue(T value);
    void UpdateBuffer() override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    kParameterBufferType BufferType() const override
    {
        return kParameterBufferType_CBV;
    }
    bool IsValid() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(m_data_)
        );
    }
};

template <typename T> requires kAllowedBufferType<T>
void ConstantBufferAsset<T>::UpdateBuffer()
{
    if (!m_is_dirty_)
    {
        return;
    }

    if (m_buffer_ == nullptr)
    {
        CreateBuffer();
    }

    m_buffer_->UpdateBuffer(&m_data_);
}

template <typename T> requires kAllowedBufferType<T>
void ConstantBufferAsset<T>::OnInspectorGui()
{
    if (Gui::PropertyField("Value", m_data_))
    {
        m_is_dirty_ = true;
    }
}

template <typename T> requires kAllowedBufferType<T>
bool ConstantBufferAsset<T>::CreateBuffer()
{
    m_buffer_ = std::make_unique<ConstantBuffer>(sizeof(T));
    return m_buffer_->CreateBuffer();
}

template <typename T> requires kAllowedBufferType<T>
void ConstantBufferAsset<T>::SetValue(T value)
{
    m_is_dirty_ = true;
    m_data_ = value;
}

template <typename T> requires kAllowedBufferType<T>
std::shared_ptr<DescriptorHandle> ConstantBufferAsset<T>::UploadBuffer()
{
    if (m_buffer_ == nullptr)
    {
        CreateBuffer();
    }

    return m_buffer_->UploadBuffer();
}

template <typename T> requires kAllowedBufferType<T>
bool ConstantBufferAsset<T>::IsValid()
{
    return m_buffer_ != nullptr && m_buffer_->IsValid();
}
}

CEREAL_CLASS_VERSION(engine::ConstantBufferAsset<int>, 1)

CEREAL_CLASS_VERSION(engine::ConstantBufferAsset<float>, 1)

CEREAL_CLASS_VERSION(engine::ConstantBufferAsset<Color>, 1)

CEREAL_CLASS_VERSION(engine::ConstantBufferAsset<Vector2>, 1)

CEREAL_CLASS_VERSION(engine::ConstantBufferAsset<Vector3>, 1)