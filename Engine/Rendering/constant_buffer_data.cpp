#include "pch.h"
#include "constant_buffer_data.h"

#include "CabotEngine/Graphics/ConstantBuffer.h"

namespace engine
{
void ConstantBufferData::AddIntData(const std::string &name)
{
    m_data_type_offset_pairs_.try_emplace(name, std::make_pair(kConstantBufferDataType::kConstantBufferDataType_Int, m_current_offset_));

    m_current_offset_ += sizeof(int);
    m_data_.resize(m_current_offset_);
}

void ConstantBufferData::AddFloatData(const std::string &name)
{
    m_data_type_offset_pairs_.try_emplace(name, std::make_pair(kConstantBufferDataType::kConstantBufferDataType_Float, m_current_offset_));

    m_current_offset_ += sizeof(float);
    m_data_.resize(m_current_offset_);
}

void ConstantBufferData::AddVector2Data(const std::string &name)
{
    m_data_type_offset_pairs_.try_emplace(name, std::make_pair(kConstantBufferDataType::kConstantBufferDataType_Vector2, m_current_offset_));

    m_current_offset_ += sizeof(Vector2);
    m_data_.resize(m_current_offset_);
}

void ConstantBufferData::AddVector3Data(const std::string &name)
{
    m_data_type_offset_pairs_.try_emplace(name, std::make_pair(kConstantBufferDataType::kConstantBufferDataType_Vector3, m_current_offset_));

    m_current_offset_ += sizeof(Vector3);
    m_data_.resize(m_current_offset_);
}

void ConstantBufferData::AddColorData(const std::string &name)
{
    m_data_type_offset_pairs_.try_emplace(name, std::make_pair(kConstantBufferDataType::kConstantBufferDataType_Color, m_current_offset_));

    m_current_offset_ += sizeof(Color);
    m_data_.resize(m_current_offset_);
}

void ConstantBufferData::AddMatrixData(const std::string &name)
{
    m_data_type_offset_pairs_.try_emplace(name, std::make_pair(kConstantBufferDataType::kConstantBufferDataType_Matrix, m_current_offset_));

    m_current_offset_ += sizeof(Matrix);
    m_data_.resize(m_current_offset_);
}

ConstantBufferData::ConstantBufferData(const ShaderParameter &shader_param) : BufferDataBase(shader_param)
{
    for (auto variable : shader_param.variables)
    {
        switch (variable.data_type)
        {
            case kConstantBufferDataType::kConstantBufferDataType_Int:
                AddIntData(variable.name);
                break;
            case kConstantBufferDataType::kConstantBufferDataType_Float:
                AddFloatData(variable.name);
                break;
            case kConstantBufferDataType::kConstantBufferDataType_Vector2:
                AddVector2Data(variable.name);
                break;
            case kConstantBufferDataType::kConstantBufferDataType_Vector3:
                AddVector3Data(variable.name);
                break;
            case kConstantBufferDataType::kConstantBufferDataType_Color:
                AddColorData(variable.name);
                break;
            case kConstantBufferDataType::kConstantBufferDataType_Matrix:
                AddMatrixData(variable.name);
                break;
            case kConstantBufferDataType::kConstantBufferDataType_Unknown:
                break;
        }
    }
}

void ConstantBufferData::OnInspectorGui()
{
    for (auto [name, data_type_offset_pair] : m_data_type_offset_pairs_)
    {
        auto offset = data_type_offset_pair.second;
        switch (data_type_offset_pair.first)
        {
            case kConstantBufferDataType::kConstantBufferDataType_Int: {
                auto data = *reinterpret_cast<int *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetIntData(name, data);
                break;
            }
            case kConstantBufferDataType::kConstantBufferDataType_Float: {
                auto data = *reinterpret_cast<float *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetFloatData(name, data);
                break;
            }
            case kConstantBufferDataType::kConstantBufferDataType_Vector2: {
                auto data = *reinterpret_cast<Vector2 *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetVector2Data(name, data);
                break;
            }
            case kConstantBufferDataType::kConstantBufferDataType_Vector3: {
                auto data = *reinterpret_cast<Vector3 *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetVector3Data(name, data);
                break;
            }
            case kConstantBufferDataType::kConstantBufferDataType_Color: {
                auto data = *reinterpret_cast<Color *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetColorData(name, data);
                break;
            }
            case kConstantBufferDataType::kConstantBufferDataType_Matrix: {
                auto data = *reinterpret_cast<Matrix *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetMatrixData(name, data);
                break;
            }
            default:
                break;
        }
    }
}

bool ConstantBufferData::SetIntData(const std::string &name, const int data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first != kConstantBufferDataType::kConstantBufferDataType_Int)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(int));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetFloatData(const std::string &name, const float data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first != kConstantBufferDataType::kConstantBufferDataType_Float)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(float));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetVector2Data(const std::string &name, const Vector2 data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first != kConstantBufferDataType::kConstantBufferDataType_Vector2)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(Vector2));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetVector3Data(const std::string &name, const Vector3 data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first != kConstantBufferDataType::kConstantBufferDataType_Vector3)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(Vector3));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetColorData(const std::string &name, const Color data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first != kConstantBufferDataType::kConstantBufferDataType_Color)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(Color));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetMatrixData(const std::string &name, const Matrix &data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first != kConstantBufferDataType::kConstantBufferDataType_Matrix)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(Matrix));

    is_dirty = true;
    return true;
}

void *ConstantBufferData::Data()
{
    return m_data_.data();
}

size_t ConstantBufferData::Size() const
{
    return m_data_.size() * sizeof(uint8_t);
}

kBufferType ConstantBufferData::BufferType()
{
    return kBufferType_ConstantBuffer;
}
}

CEREAL_REGISTER_TYPE(engine::ConstantBufferData)