#pragma once

namespace engine
{
enum kShaderType;

struct ShaderParameter
{
    int index;
    kShaderType shader_type;
    std::string name;
    std::string display_name;
    std::string type_hint;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(name, index, shader_type, type_hint);
    }
};
}