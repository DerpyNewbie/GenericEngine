#include "pch.h"
#include "constant_buffer_data.h"

#include "CabotEngine/Graphics/ConstantBuffer.h"

namespace engine
{
void ConstantBufferData::AddIntData(const std::string &name)
{
    AddData(name, sizeof(int), kConstantBufferDataType::k_Int);
}

void ConstantBufferData::AddFloatData(const std::string &name)
{
    AddData(name, sizeof(float), kConstantBufferDataType::kFloat);
}

void ConstantBufferData::AddVector2Data(const std::string &name)
{
    AddData(name, sizeof(Vector2), kConstantBufferDataType::kVector2);
}

void ConstantBufferData::AddVector3Data(const std::string &name)
{
    AddData(name, sizeof(Vector3), kConstantBufferDataType::kVector3);
}

void ConstantBufferData::AddColorData(const std::string &name)
{
    AddData(name, sizeof(Color), kConstantBufferDataType::kColor);
}

void ConstantBufferData::AddMatrixData(const std::string &name)
{
    AddData(name, sizeof(Matrix), kConstantBufferDataType::kMatrix);
}

void ConstantBufferData::AddData(const std::string &name, const size_t data_size, kConstantBufferDataType data_type)
{
    //1行に収まらなかった場合アラインメントします。
    const size_t current_row_offset = m_current_offset_ % 16;
    if (current_row_offset + data_size > 16)
        m_current_offset_ = (m_current_offset_ + 15) & ~15;

    m_data_type_offset_pairs_.try_emplace(name, std::make_pair(data_type, m_current_offset_));

    m_current_offset_ += data_size;
    m_data_.resize(m_current_offset_);
}

ConstantBufferData::ConstantBufferData(const ShaderParameter &shader_param) : BufferDataBase(shader_param)
{
    for (auto variable : shader_param.variables)
    {
        switch (variable.data_type)
        {
        case kConstantBufferDataType::k_Int:
            AddIntData(variable.name);
            break;
        case kConstantBufferDataType::kFloat:
            AddFloatData(variable.name);
            break;
        case kConstantBufferDataType::kVector2:
            AddVector2Data(variable.name);
            break;
        case kConstantBufferDataType::kVector3:
            AddVector3Data(variable.name);
            break;
        case kConstantBufferDataType::kColor:
            AddColorData(variable.name);
            break;
        case kConstantBufferDataType::kMatrix:
            AddMatrixData(variable.name);
            break;
        case kConstantBufferDataType::kUnknown:
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
        case kConstantBufferDataType::k_Int: {
            auto data = *reinterpret_cast<int *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetIntData(name, data);
                break;
            }
        case kConstantBufferDataType::kFloat: {
            auto data = *reinterpret_cast<float *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetFloatData(name, data);
                break;
            }
        case kConstantBufferDataType::kVector2: {
            auto data = *reinterpret_cast<Vector2 *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetVector2Data(name, data);
                break;
            }
        case kConstantBufferDataType::kVector3: {
            auto data = *reinterpret_cast<Vector3 *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetVector3Data(name, data);
                break;
            }
        case kConstantBufferDataType::kColor: {
            auto data = *reinterpret_cast<Color *>(m_data_.data() + offset);
                if (Gui::PropertyField(name.c_str(), data))
                    SetColorData(name, data);
                break;
            }
        case kConstantBufferDataType::kMatrix: {
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

    if (it == m_data_type_offset_pairs_.end() || it->second.first !=
        kConstantBufferDataType::k_Int)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(int));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetFloatData(const std::string &name, const float data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first !=
        kConstantBufferDataType::kFloat)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(float));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetVector2Data(const std::string &name, const Vector2 data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first !=
        kConstantBufferDataType::kVector2)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(Vector2));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetVector3Data(const std::string &name, const Vector3 data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first !=
        kConstantBufferDataType::kVector3)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(Vector3));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetColorData(const std::string &name, const Color data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first !=
        kConstantBufferDataType::kColor)
        return false;

    const auto dst = m_data_.data() + it->second.second;
    memcpy(dst, &data, sizeof(Color));

    is_dirty = true;
    return true;
}

bool ConstantBufferData::SetMatrixData(const std::string &name, const Matrix &data)
{
    const auto it = m_data_type_offset_pairs_.find(name);

    if (it == m_data_type_offset_pairs_.end() || it->second.first !=
        kConstantBufferDataType::kMatrix)
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
