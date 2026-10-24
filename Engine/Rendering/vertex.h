#pragma once
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"

namespace engine
{
/// <summary>
/// GPUに送る1つの頂点のデータです。
/// </summary>
struct Vertex
{
    Vector3 vertex;
    Color color;
    Vector3 normal;
    Vector4 tangent;
    Vector2 uvs[8];
    unsigned int bones_per_vertex = 0;
    unsigned int bone_index[4] = {};
    float bone_weight[4] = {};

    /// <summary>
    /// この構造体の並びを表す頂点レイアウトです。
    /// シェーダーの入力は POSITION, COLOR, NORMAL, TANGENT, TEXCOORD0..7, BONESPERVERTEX, BONEINDEX, BONEWEIGHT です。
    /// </summary>
    static std::shared_ptr<const CRS::VertexLayout> Layout();
};
}
