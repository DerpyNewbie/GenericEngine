struct VSOutput
{
    float4 svpos : SV_POSITION;
    float3 normal: NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 BasicPS(VSOutput input) : SV_TARGET
{
    return input.color;
}