struct VSOutput
{
    float4 svpos : SV_POSITION;
    float3 normal: NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D _MainTex : register(t3);

float4 BasicPS(VSOutput input) : SV_TARGET
{
    float3 light = normalize(float3(0.9, 0.3, -0.8));
    float brightness = dot(-light, input.normal);
    float4 mainColor = _MainTex.Sample(smp, input.uv);

    return mainColor * brightness;
}