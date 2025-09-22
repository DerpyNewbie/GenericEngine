cbuffer Transform : 
register (b0)
{
    float4x4 World;
}
cbuffer Transforms : 
register (b1)
{
    float4x4 View;
    float4x4 Proj;
}

StructuredBuffer<float4x4> BoneMatrices : 
register (t0);

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
    float4 bonePos = float4(0, 0, 0, 0);
    float3 boneNormal = float3(0, 0, 0);
    for (int i = 0; i < input.bones_per_vertex; ++i)
    {
        float weight = input.bone_weight[i];
        if (weight > 0)
        {
            float4x4 boneMatrix = BoneMatrices[input.bone_id[i]];
            float3x3 boneRot = ExtractRotation(boneMatrix);
            bonePos += mul(boneMatrix, localPos) * weight;
            boneNormal += mul(boneRot, localNormal) * weight;
        }
    }
    if (input.bones_per_vertex != 0)
    {
        localPos = bonePos;
        localNormal = boneNormal;
    }

    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    float3x3 worldRot = ExtractRotation(World);
    float3 worldNormal = mul(worldRot, localNormal);

    output.svpos = projPos;
    output.normal = normalize(float3(worldNormal.x, worldNormal.y, worldNormal.z));
    output.color = input.color;
    output.uv = input.uv;
    output.worldpos = worldPos.xyz;
    return output;
}

struct Light
{
    int type;
    int cast_shadow;
    float intensity;
    float range;

    float4 pos;
    float4 direction;
    float4 color;

    float inner_cos;
    float outer_cos;
    float2 padding;

    float4x4 view;
    float4x4 proj;
};

cbuffer LightCount : 
register (b2)
{
    int light_count;
    int a[63];
}

StructuredBuffer<Light> Lights : 
register (t1);
Texture2DArray ShadowMaps : 
register (t2);
Texture2D _MainTex : 
register (t3);
SamplerState smp : 
register (s0);
SamplerComparisonState shadowSampler : 
register (s1);

float SampleShadowPCF(float3 shadowCoord, int lightIndex)
{
    float shadow = 0.0;
    const float2 texelSize = 1.0 / float2(1920, 1065);

    [unroll]
    for (int x = -1; x <= 1; x++)
    {
        [unroll]
        for (int y = -1; y <= 1; y++)
        {
            float2 offset = float2(x, y) * texelSize;
            shadow += ShadowMaps.SampleCmpLevelZero(
                shadowSampler,
                float3(shadowCoord.xy + offset, lightIndex),
                shadowCoord.z
            );
        }
    }

    shadow /= 9.0;
    return shadow;
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

    for (int i = 0; i < light_count; ++i)
        {
        float3 L = normalize(-Lights[i].direction);
        float NdotL = saturate(dot(N, L));

        float4 worldPos = float4(input.worldpos, 1);
        float4 viewPos = mul(Lights[i].view, worldPos);
        float4 lightClip = mul(Lights[i].proj, viewPos);
        float3 shadowCoord;
            shadowCoord.xy = lightClip.xy / lightClip.w * 0.5f + 0.5f;
            shadowCoord.z = lightClip.z / lightClip.w;
        
        float shadow = 0;
        if (shadowCoord.x < 0 || shadowCoord.x > 1 ||
            shadowCoord.y < 0 || shadowCoord.y > 1)
            {
                shadow = 1.0f;
            }
        else
        {
            shadowCoord.y = 1 - shadowCoord.y;

            shadow = SampleShadowPCF(shadowCoord, i);
        }

            brightness += shadow * NdotL * Lights[i].color.rgb * Lights[i].intensity;
        }

    float2 flippedUV = float2(input.uv.x, 1.0 - input.uv.y);
    float4 mainColor = _MainTex.Sample(smp, flippedUV);

    return float4(mainColor.rgb * brightness, mainColor.a);
    }