// ライティングしないShader（板ポリやエフェクトなど）
// パラメーター: _MainTex（テクスチャ）, _Color（色）
// @Cull Off
#include "../Engine/Rendering/CaboRenderingSystem/Shaders/crs_common.hlsli"

cbuffer Material
{
    float4 _Color;
};

Texture2D _MainTex : register(t0);
SamplerState _MainTex_Sampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(StandardVertexInput input)
{
    VSOutput output;
    float3 world_normal;
    output.position = mul(view_projection, TransformToWorld(input, world_normal));
    output.uv = input.uv;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return _MainTex.Sample(_MainTex_Sampler, input.uv) * _Color;
}
