cbuffer Transform : register(b0)
{
    float4x4 World; // ワールド行列
    float4x4 View; // ビュー行列
    float4x4 Proj; // 投影行列
}

StructuredBuffer<float4x4> BoneMatrices : register(t2);

struct VSInput
{
    float3 pos : POSITION; // 頂点座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // UV
    float3 tangent : TANGENT; // 接空間
    float4 color : COLOR; // 頂点色
    uint4 bone_id : BONEID; //ボーンのイテレーター
    float4 weight : BONEWEIGHT; //ウェイト
    uint bone_num : BONENUM;
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


VSOutput BasicVS(VSInput input)
{
    VSOutput output = (VSOutput)0; // アウトプット構造体を定義する
    float4 localPos = float4(input.pos, 1.0f); // 頂点座標
    float3 localNormal = input.normal;
    float4 bonePos = float4(0, 0, 0, 0);
    float3 boneNormal = float3(0, 0, 0);
    if (input.bone_id[0] <= 255)
    {
        for (int i = 0; i < 4; ++i)
        {
            float weight = input.weight[i];
            if (weight > 0)
            {
                float4x4 boneMatrix = BoneMatrices[input.bone_id[i]];
                float3x3 boneRot =  ExtractRotation(boneMatrix);
                bonePos += mul(boneMatrix, localPos) * weight;
                boneNormal += mul(boneRot, localNormal) * weight;
            }
        }
        localPos = bonePos;
        localNormal = boneNormal;
    }
    
    float4 worldPos = mul(World, localPos); // ワールド座標に変換
    float4 viewPos = mul(View, worldPos); // ビュー座標に変換
    float4 projPos = mul(Proj, viewPos); // 投影変換

    float3x3 worldRot = ExtractRotation(World);
    float3 worldNormal = mul(worldRot,localNormal); // ワールド座標に変換

    output.svpos = projPos; // 投影変換された座標をピクセルシェーダーに渡す
    output.normal = normalize(float3(worldNormal.x, worldNormal.y, worldNormal.z));
    output.color = input.color; // 頂点色をそのままピクセルシェーダーに渡す
    output.uv = input.uv;
    return output;
}
