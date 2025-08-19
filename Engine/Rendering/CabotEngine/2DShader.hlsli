Texture2D<float4> tex : register(t2);
SamplerState smp : register(s0);

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};