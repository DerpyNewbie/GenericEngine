#pragma once
#include "buffer_base.h"
#include "gui.h"
#include "shader.h"
#include "texture_collection.h"
#include "Asset/asset_ptr.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
struct MaterialDataBase : Object, Inspectable
{
    bool is_dirty = true;
    ShaderParameter parameter;
    kGpuUploadType upload_type;

    MaterialDataBase();
    explicit MaterialDataBase(ShaderParameter param);

    void OnDeserialized() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(parameter),
            CEREAL_NVP(upload_type)
        );
    }
};

inline MaterialDataBase::MaterialDataBase() :
    parameter()
{}

inline MaterialDataBase::MaterialDataBase(ShaderParameter param) :
    parameter(std::move(param))
{}

inline void MaterialDataBase::OnDeserialized()
{
    is_dirty = true;
}
}

CEREAL_CLASS_VERSION(engine::MaterialDataBase, 1)