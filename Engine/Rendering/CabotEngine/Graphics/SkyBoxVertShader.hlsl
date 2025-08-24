cbuffer Transforms : register(b1)
{
    float4x4 View;
    float4x4 Proj;
}

struct VSInput
{
    float3 pos : POSITION; // 立方体の頂点座標
};

struct VSOutput
{
    float4 svpos : SV_POSITION; // 投影後のクリップ座標
    float3 dir : TEXCOORD0; // キューブマップ用の方向ベクトル
};

float4x4 RemoveTranslation(float4x4 view)
{
    float4x4 result = view;

    // 4列目 (translation部分) をゼロにする
    result[0][3] = 0;
    result[1][3] = 0;
    result[2][3] = 0;

    // 齢はそのまま(同次座標用のw)
    result[3][3] = 1;

    return result;
}

VSOutput vert(VSInput input)
{
    VSOutput output;

    // View の平行移動だけを消す
    float4x4 viewNoTranslation = RemoveTranslation(View);
    // VP合成
    float4x4 vp = mul(Proj, viewNoTranslation);

    // 頂点変換
    float4 clipPos = mul(vp, float4(input.pos, 1.0f));

    // 常に奥へ
    clipPos.z = clipPos.w;
    output.svpos = clipPos;

    // サンプル方向
    output.dir = input.pos;

    return output;
}