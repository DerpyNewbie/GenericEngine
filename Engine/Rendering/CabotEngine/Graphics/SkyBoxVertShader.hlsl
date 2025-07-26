cbuffer CameraBuffer : register(b0)
{
    float4x4 viewProj;
}

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
    float4 pos : SV_POSITION;
    float3 dir : TEXCOORD;
};

VSOutput vert(VSInput input)
{
    VSOutput output;

    // カメラ位置は無視（回転のみ）
    float4 worldPos = float4(input.pos, 1.0f);
    float4 clipPos = mul(viewProj, worldPos);
    clipPos.z = clipPos.w; // z=1.0に固定
    output.pos = clipPos;

    output.dir = input.pos; // カメラ方向ベクトル
    return output;
}