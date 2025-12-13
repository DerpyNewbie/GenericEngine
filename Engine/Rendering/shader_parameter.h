#pragma once
#include "shader_type.h"

namespace engine
{
struct ShaderParameter
{
    int index;
    kShaderType shader_type;
    std::string name;
    std::string display_name;
    std::string type_hint;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(index),
            CEREAL_NVP(shader_type),
            CEREAL_NVP(name),
            CEREAL_NVP(display_name),
            CEREAL_NVP(type_hint)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ShaderParameter, 1)