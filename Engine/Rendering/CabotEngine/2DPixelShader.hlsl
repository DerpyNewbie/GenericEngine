#include "2DShader.hlsli"

float4 pix(VSOutput input) : SV_TARGET
{
    float4 mainColor = tex.Sample(smp, input.uv);
    float Y = dot(mainColor.rgb, float3(0.299, 0.587, 0.114));
    return float4(Y, Y, Y, 1);
}