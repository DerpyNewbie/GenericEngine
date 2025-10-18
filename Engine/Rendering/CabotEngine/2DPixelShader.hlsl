#include "2DShader.hlsli"

float4 pix(VSOutput input) : SV_TARGET
{
    float2 flippedUV = float2(input.uv.x, 1.0 - input.uv.y);
    float4 mainColor = tex.Sample(smp, flippedUV);
    return mainColor;
}