#include "2DShader.hlsli"

float4 pix(VSOutput input) : SV_TARGET
{
    float4 mainColor = tex.Sample(smp, input.uv);
    return mainColor;
}