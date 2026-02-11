#pragma once

namespace engine
{
struct ShaderParameter
{
    int index;
    std::string name;
    std::string display_name;
    std::string type_hint;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(index),
            CEREAL_NVP(name),
            CEREAL_NVP(display_name),
            CEREAL_NVP(type_hint)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ShaderParameter, 1)