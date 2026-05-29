#pragma once
#include "buffer_base.h"
#include "gpu_resource_group.h"
#include "inspectable.h"
#include "object.h"
#include "shader_parameter.h"

namespace engine
{
class ComputeShader : public Object, public Inspectable
{
    friend class ComputeShaderImporter;

    ComPtr<ID3DBlob> m_shader_blob_;

public:
    std::vector<ShaderParameter> parameters;

    void OnInspectorGui() override;

    CD3DX12_SHADER_BYTECODE GetByteCode() const
    {
        return m_shader_blob_.Get();
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ComputeShader, 1)