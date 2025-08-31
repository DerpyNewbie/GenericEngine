cbuffer Transform : register(b0)
{
    float4x4 World;
}
cbuffer Transforms : register(b1)
{
    float4x4 View;
    float4x4 Proj;
}

StructuredBuffer<float4x4> BoneMatrices : register(t0);

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
    float3 normal: NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
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
    if(input.bones_per_vertex != 0)
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
    return output;
}

SamplerState smp : register(s0);
Texture2D _MainTex : register(t1);

float4 pix(VSOutput input) : SV_TARGET
{
    float3 light = normalize(float3(0.9, 0.3, -0.8));
    float brightness = clamp(dot(-light,input.normal),0,1);
    float2 flippedUV = clamp(float2(input.uv.x, 1.0 - input.uv.y),0,1);
    float4 mainColor = _MainTex.Sample(smp, flippedUV);

    return mainColor * brightness;
}
