#pragma once

struct Vertex
{
public:
    Vector3 vertex;
    Color color;
    Vector3 normal;
    Vector4 tangent;
    Vector2 uvs[8];
    unsigned int bones_per_vertex;
    unsigned int bone_index[4];
    float bone_weight[4];

    static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
    static constexpr int InputElementCount = 15;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};