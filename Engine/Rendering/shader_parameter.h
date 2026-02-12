#pragma once

namespace engine
{
struct ShaderParameter
{
    int index;
    std::string name;
    std::string display_name;
    std::string type_hint;

    bool operator==(const ShaderParameter &other) const
    {
        // do not check for `display_name` or `type_hint` because only real parameters are `index` and `name`
        return index == other.index && name == other.name;
    }

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