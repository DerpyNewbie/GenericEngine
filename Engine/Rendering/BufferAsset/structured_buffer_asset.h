#pragma once
#include "gui.h"
#include "allowed_buffer_type.h"
#include "../CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
template <typename T> requires kAllowedBufferType<T>
class StructuredBufferAsset : public IBuffer
{
    std::unique_ptr<StructuredBuffer> m_buffer_ = nullptr;
    bool m_is_dirty_ = true;
    std::vector<T> m_data_;

public:
    void OnInspectorGui() override;
    bool CreateBuffer() override;
    void UpdateBuffer() override;
    void SetValue(std::vector<T> value);
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

template <typename T> requires kAllowedBufferType<T>
void StructuredBufferAsset<T>::UpdateBuffer()
{
    if (m_buffer_ == nullptr)
    {
        CreateBuffer();
    }

    m_is_dirty_ = false;
    m_buffer_->UpdateBuffer(m_data_.data());
}

template <typename T> requires kAllowedBufferType<T>
void StructuredBufferAsset<T>::OnInspectorGui()
{
    for (size_t i = 0; i < m_data_.size(); ++i)
    {
        if (Gui::PropertyField("Value ", m_data_[i]))
        {
            m_is_dirty_ = true;
        }
    }
    if (ImGui::Button("Add Element"))
    {
        m_data_.emplace_back();
        CreateBuffer();
        m_is_dirty_ = true;
    }
}

template <typename T> requires kAllowedBufferType<T>
bool StructuredBufferAsset<T>::CreateBuffer()
{
    m_buffer_ = std::make_unique<StructuredBuffer>(sizeof(T), static_cast<uint32_t>(m_data_.size()));
    return m_buffer_->CreateBuffer();
}

template <typename T> requires kAllowedBufferType<T>
void StructuredBufferAsset<T>::SetValue(std::vector<T> value)
{
    m_is_dirty_ = true;
    m_data_ = std::move(value);
}

template <typename T> requires kAllowedBufferType<T>
std::shared_ptr<DescriptorHandle> StructuredBufferAsset<T>::UploadBuffer()
{
    if (m_buffer_ == nullptr)
    {
        CreateBuffer();
    }

    if (m_is_dirty_)
    {
        UpdateBuffer();
    }

    return m_buffer_->UploadBuffer();
}

template <typename T> requires kAllowedBufferType<T>
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