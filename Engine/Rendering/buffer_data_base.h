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
struct BufferDataBase : Inspectable
{
    bool is_dirty = true;
    ShaderParameter parameter;
    kGpuUploadType upload_type;

    BufferDataBase() = default;
    explicit BufferDataBase(const ShaderParameter &param);

    virtual kBufferType BufferType() = 0;
    
    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(parameter),
            CEREAL_NVP(upload_type)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::BufferDataBase, 1)