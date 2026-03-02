#include "Light.hlsli"

cbuffer Transform : register (b0)
{
    float4x4 World;
}
cbuffer Transforms : register (b1)
{
    float4x4 View;
    float4x4 Proj;
}

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct VSInput
{
    float3 pos : POSITION; // 頂点座標
    float4 color : COLOR; // 頂点色
    float3 normal : NORMAL; // 法線
    float4 tangent : TANGENT; // 接空間
    float2 uv1 : TEXCOORD; // UV
    float2 uv2 : TEXCOORD1;
    float2 uv3 : TEXCOORD2;
    float2 uv4 : TEXCOORD3;
    float2 uv5 : TEXCOORD4;
    float2 uv6 : TEXCOORD5;
    float2 uv7 : TEXCOORD6;
    float2 uv8 : TEXCOORD7;
    uint bones_per_vertex : BONESPERVERTEX;
    uint4 bone_id : BONEINDEX;
    float4 bone_weight : BONEWEIGHT;
};

VSOutput vrt(VSInput input)
{
    VSOutput output;
    
    float4 worldPos = mul(World, float4(input.pos, 1.0f));
    
    output.svpos = worldPos;
    output.uv = input.uv1;

    return output;
}

float4 pix(VSOutput input) : SV_TARGET
{
    float4 mainColor = _MainTex.Sample(smp, input.uv);
    return mainColor;
}