cbuffer Transform : register(b0)
{
    float4x4 World;
}
cbuffer Transforms : register(b1)
{
    float4x4 View;
    float4x4 Proj;
}

struct VSInput
{
    float3 pos : POSITION; // 頂点座標
    float4 color : COLOR; // 頂点色
    float3 normal : NORMAL; // 法線
    float4 tangent : TANGENT; // 接空間
    float2 uv1 : TEXCOORD0; // UV
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
Texture2DArray<float> ShadowMaps : register(t2);
Texture2D _MainTex : register(t3);

float LinearizeDepth(float depth, float nearZ, float farZ)
{
    return nearZ * farZ / (farZ - depth * (farZ - nearZ));
}

float4 pix(VSOutput input) : SV_TARGET
{
    float2 flippedUV = clamp(float2(input.uv.x, 1.0 - input.uv.y),0,1);
    float depth = ShadowMaps.Sample(smp, float3(flippedUV.xy, 1));
    float linearDepth = LinearizeDepth(depth, 0.1f, 5.0f);

    // [0,1] に正規化して可視化
    float normalized = saturate(linearDepth / 5.0f);
    return float4(normalized, normalized, normalized, 1);
}