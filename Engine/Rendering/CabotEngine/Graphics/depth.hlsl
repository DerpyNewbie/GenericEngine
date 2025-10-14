#define SHADOW_CASCADE_COUNT 3

cbuffer Transform : register (b0)
{
    float4x4 World;
}
cbuffer LightCount : register (b3)
{
    int light_count;
    int a[63];
}

StructuredBuffer<float4x4> LightViewProj : register(t1);

StructuredBuffer<float4x4> BoneMatrices : register (t0);

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
    float2 uv3 : TEXCOORD2;
    float2 uv4 : TEXCOORD3;
    float2 uv5 : TEXCOORD4;
    float2 uv6 : TEXCOORD5;
    float2 uv7 : TEXCOORD6;
    float2 uv8 : TEXCOORD7;
    uint bones_per_vertex : BONESPERVERTEX;
    uint4 bone_id : BONEINDEX;
    float4 bone_weight : BONEWEIGHT;
};

float4 vrt(VSInput input) : SV_POSITION
{
    float4 localPos = float4(input.pos, 1.0f);
    float4 skinnedPos = 0;

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        float w = input.bone_weight[i];
        if (w > 0)
        {
            float4x4 boneMatrix = BoneMatrices[input.bone_id[i]];
            skinnedPos += mul(boneMatrix, localPos) * w;
        }
    }

    if (input.bones_per_vertex != 0)
        localPos = skinnedPos;

    float4 worldPos = mul(World, localPos);
    float4 projPos = mul(LightViewProj[light_count], worldPos);

    return projPos;
}