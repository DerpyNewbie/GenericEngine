#pragma once
#include "bone_weight.h"
#include "Math/color.h"
#include "Math/vector2.h"
#include "Math/vector3.h"
#include "Math/vector4.h"

#include <d3d12.h>

struct Vertex
{
public:
    engine::Vector3 vertex;
    engine::Color color;
    engine::Vector2 uvs[8];
    engine::Vector3 normal;
    engine::Vector4 tangent;
    unsigned char bones_per_vertex;
    unsigned int bone_index[4];
    float bone_weight[4];

    static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
    static constexpr int InputElementCount = 15;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};
