#pragma once
#include "object.h"

namespace engine
{
class Texture2D;
/// <summary>
/// Shared shader parameters.
/// </summary>
/// <remarks>
/// Used by Material for better memory-management among the same objects
/// </remarks>
class MaterialBlock : public Object
{
public:
    std::unordered_map<std::string, float> params_float;
    std::unordered_map<std::string, int> params_int;
    std::unordered_map<std::string, bool> params_bool;
    std::unordered_map<std::string, Vector2> params_vec2;
    std::unordered_map<std::string, Vector3> params_vec3;
    std::unordered_map<std::string, Vector4> params_vec4;
    std::unordered_map<std::string, Matrix> params_mat4;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> params_tex2d;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this),
           CEREAL_NVP(params_float), CEREAL_NVP(params_int), CEREAL_NVP(params_bool),
           CEREAL_NVP(params_vec2), CEREAL_NVP(params_vec3), CEREAL_NVP(params_vec4), CEREAL_NVP(params_mat4),
           CEREAL_NVP(params_tex2d));
    }
};
}