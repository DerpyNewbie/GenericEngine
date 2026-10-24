#include "crs_common.hlsli"

// 標準のShader（ライティングと影あり）
// Materialのパラメーター: _MainTex（テクスチャ）, _Color（色）

cbuffer Material
{
    float4 _Color;
};

Texture2D _MainTex : register(t0);
SamplerState _MainTex_Sampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 world_position : TEXCOORD1;
};

VSOutput VSMain(StandardVertexInput input)
{
    VSOutput output;
    float3 world_normal;
    const float4 world_position = TransformToWorld(input, world_normal);
    output.position = mul(view_projection, world_position);
    output.normal = world_normal;
    output.uv = input.uv;
    output.world_position = world_position.xyz;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    const float4 albedo = _MainTex.Sample(_MainTex_Sampler, input.uv) * _Color;
    const float3 lighting = CalcLighting(normalize(input.normal), input.world_position);
    return float4(albedo.rgb * lighting, albedo.a);
}

// 影用。深度だけを書きます（view_projectionはライトの行列）
float4 VSShadow(StandardVertexInput input) : SV_POSITION
{
    float3 world_normal;
    const float4 world_position = TransformToWorld(input, world_normal);
    return mul(view_projection, world_position);
}
