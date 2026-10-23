#include "crs_common.hlsli"

struct LineVertex
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VSMain(LineVertex input)
{
    VSOutput output;
    output.position = mul(view_projection, float4(input.position, 1.0f));
    output.color = input.color;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return input.color;
}
