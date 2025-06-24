cbuffer Transform : register(b0)
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

VSOutput main(VSInput input)
{
    float4 localPos = float4(input.pos, 1.0f);
    float4 viewPos = mul(View, localPos);
    float4 projPos = mul(Proj, viewPos);

    VSOutput output;
    output.svpos = projPos;
    output.normal = input.pos;
    output.color = input.color;
    output.uv = float2(1,1);
    
	return output;
}