#pragma once
#include "constant_buffer_data.h"
#include "buffer_data_base.h"
#include "byte_address_buffer_data.h"
#include "shader.h"
#include "structured_buffer_data.h"
#include "texture_buffer_data.h"
#include "uav_texture_buffer_data.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
class MaterialBlock : public Object, public Inspectable
{
    friend class GpuResourceManager;

    std::unordered_map<std::string, std::shared_ptr<BufferDataBase>> m_buffer_data_;

public:
    MaterialBlock() = default;

void OnInspectorGui() override;

    std::shared_ptr<ConstantBufferData> GetConstantBufferData(const std::string &name);
    std::shared_ptr<StructuredBufferData> GetStructuredBufferData(const std::string &name);
    std::shared_ptr<TextureBufferData> GetTextureBufferData(const std::string &name);
    std::shared_ptr<UavTextureBufferData> GetUavTextureBufferData(const std::string &name);
    std::shared_ptr<ByteAddressBufferData> GetByteAddressBufferData(const std::string &name);

    void LoadShaderParameters(const std::vector<ShaderParameter> &shader_params);
    
    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(m_buffer_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::MaterialBlock, 2)