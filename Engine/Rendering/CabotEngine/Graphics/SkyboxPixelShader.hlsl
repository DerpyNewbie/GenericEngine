TextureCube skyboxTexture : register(t1);
SamplerState linearSampler : register(s0);

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 dir : TEXCOORD;
};

float4 pix(VSOutput input) : SV_Target
{
    return skyboxTexture.Sample(linearSampler, normalize(input.dir));
}