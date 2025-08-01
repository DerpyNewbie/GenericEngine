cbuffer Transform : register(b0)
{
    float4x4 World; // ワールド行列
    float4x4 View; // ビュー行列
    float4x4 Proj; // 投影行列
}

cbuffer DebugType : register(b1)
{
    int debug_type;
}

cbuffer WeightIndex : register(b2)
{
    int weight_idx;
}

StructuredBuffer<float4x4> BoneMatrices : register(t0);

struct VSInput
{
    float3 pos : POSITION; // 頂点座標
    float4 color : COLOR; // 頂点色
    float3 normal : NORMAL; // 法線
    float4 tangent : TANGENT; // 接空間
    float2 uv : TEXCOORD0; // UV
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
    float4 svpos : SV_POSITION; // 変換された座標
    float3 normal: NORMAL;
    float4 color : COLOR; // 変換された色
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

VSOutput vrt(VSInput input)
{
    VSOutput output = (VSOutput)0; // アウトプット構造体を定義する
    float4 localPos = float4(input.pos, 1.0f); // 頂点座標
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

    localPos = bonePos;
    localNormal = boneNormal;

    float4 worldPos = mul(World, localPos); // ワールド座標に変換
    float4 viewPos = mul(View, worldPos); // ビュー座標に変換
    float4 projPos = mul(Proj, viewPos); // 投影変換

    float3x3 worldRot = ExtractRotation(World);
    float3 worldNormal = mul(worldRot, localNormal); // ワールド座標に変換

    switch (debug_type)
    {
    case 0:
        input.color = float4(input.uv, 0.0f, 1.0f);
        break;
    case 1:
        input.color = float4(input.normal, 1.0f);
        break;
    case 2:
        input.color = float4(input.bone_weight[weight_idx], 0.0f, 0.0f, 1.0f);
        break;
	default:
		input.color = float4(1,0,1,1);
		break;
    }

    output.svpos = projPos; // 投影変換された座標をピクセルシェーダーに渡す
    output.normal = normalize(float3(worldNormal.x, worldNormal.y, worldNormal.z));
    output.color = input.color; // 頂点色をそのままピクセルシェーダーに渡す
    output.uv = input.uv;
    return output;
}

SamplerState smp : register(s0);

float4 pix(VSOutput input) : SV_TARGET
{
    float3 light = normalize(float3(0.9, 0.3, -0.8));
    float brightness = (dot(-light, input.normal) + 1) / 2;

    return input.color;
}