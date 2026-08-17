#pragma once
#include "buffer_data_base.h"

namespace engine
{
class ConstantBufferData : public BufferDataBase
{
    size_t m_current_offset_ = 0;
    using DataTypeOffsetPair = std::pair<kConstantBufferDataType, size_t>;
    std::unordered_map<std::string, DataTypeOffsetPair> m_data_type_offset_pairs_;
    std::vector<uint8_t> m_data_;

public:
    ConstantBufferData() = default;
    ConstantBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    void AddData(const std::string &name, size_t data_size, kConstantBufferDataType data_type);
    void AddIntData(const std::string &name);
    void AddFloatData(const std::string &name);
    void AddVector2Data(const std::string &name);
    void AddVector3Data(const std::string &name);
    void AddColorData(const std::string &name);
    void AddMatrixData(const std::string &name);
    
    bool SetIntData(const std::string &name, int data);
    bool SetFloatData(const std::string &name, float data);
    bool SetVector2Data(const std::string &name, Vector2 data);
    bool SetVector3Data(const std::string &name, Vector3 data);
    bool SetColorData(const std::string &name, Color data);
    bool SetMatrixData(const std::string &name, const Matrix &data);

    void *Data();

    size_t Size() const;
    kBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<BufferDataBase>(this),
            CEREAL_NVP(m_data_type_offset_pairs_),
            CEREAL_NVP(m_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ConstantBufferData, 1)