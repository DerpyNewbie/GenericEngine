#pragma once
#include "ibuffer.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "gui.h"

template <typename T>
concept kAllowedBufferType =
    std::same_as<T, int> ||
    std::same_as<T, float> ||
    std::same_as<T, Color> ||
    std::same_as<T, Vector2> ||
    std::same_as<T, Vector3>;

namespace engine
{
template <kAllowedBufferType T>
class ConstantBufferAsset : public IBuffer
{
    std::unique_ptr<ConstantBuffer> m_buffer_ = nullptr;
    T m_data_;

public:
    void OnInspectorGui() override;
    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
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

template <kAllowedBufferType T>
void ConstantBufferAsset<T>::OnInspectorGui()
{
    if (Gui::PropertyField("Value", m_data_))
    {
        UpdateBuffer(&m_data_);
    }
}

template <kAllowedBufferType T>
void ConstantBufferAsset<T>::CreateBuffer()
{
    m_buffer_ = std::make_unique<ConstantBuffer>(sizeof(T));
    m_buffer_->CreateBuffer();
}

template <kAllowedBufferType T>
void ConstantBufferAsset<T>::UpdateBuffer(void *data)
{
    if (m_buffer_ == nullptr)
    {
        CreateBuffer();
    }

    m_buffer_->UpdateBuffer(data);
}

template <kAllowedBufferType T>
std::shared_ptr<DescriptorHandle> ConstantBufferAsset<T>::UploadBuffer()
{
    if (m_buffer_ == nullptr)
    {
        CreateBuffer();
    }

    return m_buffer_->UploadBuffer();
}

template <kAllowedBufferType T>
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