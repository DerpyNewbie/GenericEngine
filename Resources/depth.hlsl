#define SHADOW_CASCADE_COUNT 3

cbuffer Transform : register (b0)
{
    float4x4 World;
}
cbuffer LightCount : register (b4)
{
    int light_count;
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

struct VSOutput
{
    float3 world_pos : WORLDPOS;
};

VSOutput vrt(VSInput input)
{
    VSOutput output = (VSOutput)0;

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

    output.world_pos = worldPos.xyz;

    return output;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 near_far : NEARFAR;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

float LinearizeDepth(float depth, float nearZ, float farZ)
{
    return (nearZ * farZ) / (farZ - depth * (farZ - nearZ));
}

float pix(GSOutput input) : SV_TARGET
{
    float linearDepth = LinearizeDepth(input.pos.w, input.near_far.x, input.near_far.y);
    
    return linearDepth / input.near_far.y;
}

#define MAX_SHADOWMAP_COUNT 10

[maxvertexcount(3 * MAX_SHADOWMAP_COUNT)]
void geo(triangle VSOutput input[3], inout TriangleStream<GSOutput> tri_stream)
{
    for (int i = 0; i < MAX_SHADOWMAP_COUNT; ++i)
    {
        float4x4 viewproj = LightViewProj[i];
        for (uint j = 0; j < 3; j++)
        {
            GSOutput element;
            element.pos = mul(viewproj, float4(input[j].world_pos, 1));
            element.near_far = float2(0.1f, 10.0f);
            element.RTIndex = i;
            tri_stream.Append(element);
        }
        tri_stream.RestartStrip();
    }
}