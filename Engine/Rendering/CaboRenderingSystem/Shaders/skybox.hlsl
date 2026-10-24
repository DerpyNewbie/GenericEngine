#include "crs_common.hlsli"

TextureCube skybox_texture : register(t0);
SamplerState skybox_sampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 direction : TEXCOORD0;
};

// view_projectionには、平行移動を除いたViewとProjectionの積が入っています
VSOutput VSMain(float3 position : POSITION)
{
    VSOutput output;
    const float4 clip = mul(view_projection, float4(position, 1.0f));
    // 一番奥に置く
    output.position = clip.xyww;
    output.direction = position;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return skybox_texture.Sample(skybox_sampler, normalize(input.direction));
}
