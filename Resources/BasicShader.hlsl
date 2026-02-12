#include "Light.hlsli"

cbuffer WorldMatrix : register (b0)
{
    float4x4 World;
}
cbuffer ViewProjMatrix : register (b1)
{
    float4x4 View;
    float4x4 Proj;
}

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

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 worldpos : TEXCOORD1;
};

float3x3 ExtractRotation(float4x4 m)
{
    return float3x3(
        m[0].xyz,
        m[1].xyz,
        m[2].xyz
        );
}

static const float4x4 identityMatrix = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

VSOutput vrt(VSInput input)
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4(input.pos, 1.0f);
    float3 localNormal = input.normal;

    float4 skinnedPos = 0;
    float3 skinnedNormal = 0;

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        float w = input.bone_weight[i];
        if (w > 0)
        {
            float4x4 boneMatrix = BoneMatrices[input.bone_id[i]];
            skinnedPos += mul(boneMatrix, localPos) * w;
            skinnedNormal += mul((float3x3)boneMatrix, localNormal) * w;
        }
    }

    if (input.bones_per_vertex != 0)
    {
        localPos = skinnedPos;
        localNormal = skinnedNormal;
    }

    float4 worldPos = mul(World, localPos);
    float4 projPos = mul(Proj, mul(View, worldPos));

    float3 worldNormal = mul((float3x3)World, localNormal);

    output.svpos = projPos;
    output.normal = normalize(worldNormal);
    output.color = input.color;
    output.uv = input.uv;
    output.worldpos = worldPos.xyz;
    return output;
}

float4 pix(VSOutput input) : SV_TARGET
{
    float3 N = normalize(input.normal);
    float3 brightness = float3(0, 0, 0);
    if (light_count == 0)
    {
        float2 flippedUV = float2(input.uv.x, 1.0 - input.uv.y);
        float4 mainColor = _MainTex.Sample(smp, flippedUV);
        return float4(mainColor.rgb, mainColor.a);
    }

    float4 viewPos = mul(View, float4(input.worldpos, 1.0));
    float depth = abs(viewPos.z);

    int cascade_index = 0;
    for (int i = 0; i < SHADOW_CASCADE_COUNT; ++i)
    {
        if (depth < cascade_slices[i])
            cascade_index = 0;
    }

    int current_shadowmap_count = 0;
    int itr = current_shadowmap_count * SHADOW_CASCADE_COUNT + cascade_index;
    for (int i = 0; i < light_count; ++i)
    {
        switch (Lights[i].type)
        {
        case 0:
            brightness += CalcDirectionalShadow(Lights[i],N,input.worldpos,current_shadowmap_count);
            current_shadowmap_count += 3;
            break;
				case 1:
						brightness += CalcSpotShadow(Lights[i],N,input.worldpos,current_shadowmap_count);
						current_shadowmap_count += 1;
            break;
        default:
            break;
        }
    }

    float4 main_color = _MainTex.Sample(smp, input.uv);

    return float4(main_color.rgb * brightness, main_color.a);
}