TextureCube skyboxTexture : register(t3);
SamplerState linearSampler : register(s0);

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 dir : TEXCOORD;
};

float4 pix(VSOutput input) : SV_Target
{
    return skyboxTexture.Sample(linearSampler, normalize(float3(input.dir.x, input.dir.y, -input.dir.z)));
}