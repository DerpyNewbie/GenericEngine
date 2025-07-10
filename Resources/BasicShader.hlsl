cbuffer Transform : register(b0)
{
    float4x4 World; // ワールド行列
    float4x4 View; // ビュー行列
    float4x4 Proj; // 投影行列
}

StructuredBuffer<float4x4> BoneMatrices : register(t0);

struct VSInput
{
    float3 pos : POSITION; // 頂点座標
    float4 color : COLOR; // 頂点色
    float3 normal : NORMAL; // 法線
    float4 tangent : TANGENT; // 接空間
    float2 uv1 : TEXCOOD0; // UV
    float2 uv2 : TEXCOOD1;
    float2 uv3 : TEXCOOD2;
    float2 uv4 : TEXCOOD3;
    float2 uv5 : TEXCOOD4;
    float2 uv6 : TEXCOOD5;
    float2 uv7 : TEXCOOD6;
    float2 uv8 : TEXCOOD7;
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
    /*for (int i = 0; i < input.bones_per_vertex; ++i)
    {
        float weight = input.bone_weight[i];
        if (weight > 0)
        {
            float4x4 boneMatrix = BoneMatrices[input.bone_id[i]];
            float3x3 boneRot = ExtractRotation(boneMatrix);
            bonePos += mul(boneMatrix, localPos) * weight;
            boneNormal += mul(boneRot, localNormal) * weight;
        }
        localPos = bonePos;
        localNormal = boneNormal;
    }*/
    
    float4 worldPos = mul(World, localPos); // ワールド座標に変換
    float4 viewPos = mul(View, worldPos); // ビュー座標に変換
    float4 projPos = mul(Proj, viewPos); // 投影変換
    
    float3x3 worldRot = ExtractRotation(World);
    float3 worldNormal = mul(worldRot, localNormal); // ワールド座標に変換
    
    output.svpos = projPos; // 投影変換された座標をピクセルシェーダーに渡す
    output.normal = normalize(float3(worldNormal.x, worldNormal.y, worldNormal.z));
    output.color = input.color; // 頂点色をそのままピクセルシェーダーに渡す
    output.uv = input.uv1;
    return output;
}

SamplerState smp : register(s0);
Texture2D _MainTex : register(t0);

float4 pix(VSOutput input) : SV_TARGET
{
    float3 light = normalize(float3(0.9, 0.3, -0.8));
    float brightness = dot(-light,input.normal);
    float4 mainColor = _MainTex.Sample(smp, input.uv);

    return float4(1,1,1,1) * brightness;
}
