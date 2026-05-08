#pragma once
#include "gui.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"

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
class StructuredBufferAsset : public IBuffer
{
    std::unique_ptr<StructuredBuffer> m_buffer_ = nullptr;
    std::vector<T> m_data_;

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
void StructuredBufferAsset<T>::OnInspectorGui()
{
    for (size_t i = 0; i < m_data_.size(); ++i)
    {
        if (Gui::PropertyField("Value " + std::to_string(i), m_data_[i]))
        {
            UpdateBuffer(m_data_.data());
        }
    }
    if (ImGui::Button("Add Element"))
    {
        m_data_.emplace_back();
        CreateBuffer();
        UpdateBuffer(m_data_.data());
    }
}

template <kAllowedBufferType T>
void StructuredBufferAsset<T>::CreateBuffer()
{
    m_buffer_ = std::make_unique<StructuredBuffer>(sizeof(T), static_cast<uint32_t>(m_data_.size()));
    m_buffer_->CreateBuffer();
}

template <kAllowedBufferType T>
void StructuredBufferAsset<T>::UpdateBuffer(void *data)
{
    if (m_buffer_ == nullptr)
    {
        CreateBuffer();
    }

    m_buffer_->UpdateBuffer(data, sizeof(T) * m_data_.size());
}

template <kAllowedBufferType T>
std::shared_ptr<DescriptorHandle> StructuredBufferAsset<T>::UploadBuffer()
{
    if (m_buffer_ == nullptr)
    {
        CreateBuffer();
    }

    return m_buffer_->UploadBuffer();
}

template <kAllowedBufferType T>
bool StructuredBufferAsset<T>::IsValid()
{
    return m_buffer_ != nullptr && m_buffer_->IsValid();
}
}

CEREAL_CLASS_VERSION(engine::StructuredBufferAsset<int>, 1)

CEREAL_CLASS_VERSION(engine::StructuredBufferAsset<float>, 1)

CEREAL_CLASS_VERSION(engine::StructuredBufferAsset<Color>, 1)

CEREAL_CLASS_VERSION(engine::StructuredBufferAsset<Vector2>, 1)

CEREAL_CLASS_VERSION(engine::StructuredBufferAsset<Vector3>, 1)