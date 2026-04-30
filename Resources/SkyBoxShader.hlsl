cbuffer Transforms : register(b1)
{
    float4x4 View;
    float4x4 Proj;
}

struct VSInput
{
    float3 pos : POSITION;
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float3 dir : TEXCOORD0;
};

float4x4 RemoveTranslation(float4x4 view)
{
    float4x4 result = view;

    result[0][3] = 0;
    result[1][3] = 0;
    result[2][3] = 0;

    result[3][3] = 1;

    return result;
}

VSOutput vrt(VSInput input)
{
    VSOutput output;

    float4x4 viewNoTranslation = RemoveTranslation(View);

    float4x4 vp = mul(Proj, viewNoTranslation);

    float4 clipPos = mul(vp, float4(input.pos, 1.0f));

    clipPos.z = clipPos.w;
    output.svpos = clipPos;

    output.dir = input.pos;

    return output;
}

TextureCube skyboxTexture : register(t4);
SamplerState linearSampler : register(s0);

float4 pix(VSOutput input) : SV_Target
{
    return skyboxTexture.Sample(linearSampler, normalize(float3(input.dir.x, input.dir.y, -input.dir.z)));
}