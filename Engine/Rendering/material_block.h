#pragma once
#include <directx/d3dx12.h>
#include <ranges>
#include "object.h"
#include "CabotEngine/Graphics/Texture2D.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
/// <summary>
/// Shared shader parameters.
/// </summary>
/// <remarks>
/// Used by Material for better memory-management among the same objects
/// </remarks>
class MaterialBlock : public Object
{
    template <typename T>
    std::vector<T> GetValues(const std::vector<std::pair<std::string, T>>& pairs);

public:
    std::vector<std::pair<std::string, float>> params_float;
    std::vector<std::pair<std::string, int>> params_int;
    std::vector<std::pair<std::string, Vector2>> params_vec2;
    std::vector<std::pair<std::string, Vector3>> params_vec3;
    std::vector<std::pair<std::string, Vector4>> params_vec4;
    std::vector<std::pair<std::string, Matrix>> params_mat4;
    std::vector<std::pair<std::string, std::shared_ptr<Texture2D>>> params_tex2d;

    StructuredBuffer<float> params_float_buffer;
    StructuredBuffer<int> params_int_buffer;
    StructuredBuffer<Vector2> params_vec2_buffer;
    StructuredBuffer<Vector3> params_vec3_buffer;
    StructuredBuffer<Vector4> params_vec4_buffer;
    StructuredBuffer<Matrix> params_mat4_buffer;

    void CreateBuffer();
    void UpdateBuffer();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this),
           CEREAL_NVP(params_float), CEREAL_NVP(params_int),
           CEREAL_NVP(params_vec2), CEREAL_NVP(params_vec3), CEREAL_NVP(params_vec4), CEREAL_NVP(params_mat4),
           CEREAL_NVP(params_tex2d));
    }
};

template <typename T>
std::vector<T> MaterialBlock::GetValues(const std::vector<std::pair<std::string, T>>& pairs)
{
    auto result = pairs | std::ranges::views::values;
    return std::vector<T>(result.begin(), result.end());
}
}