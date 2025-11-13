struct GSOutput
{
    float4 pos : SV_POSITION; // clip space
    float2 near_far : NEARFAR;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

float LinearizeDepth(float depth, float nearZ, float farZ)
{
    // depth は [0, 1] の範囲に正規化された z/w
    // これを view 空間距離（線形）に変換
    return (nearZ * farZ) / (farZ - depth * (farZ - nearZ));
}

float pix(GSOutput input) : SV_TARGET
{
    // 非線形深度を取得（z/wを[0,1]に）
    float depth = input.pos.z / input.pos.w;

    // 線形化
    float linearDepth = LinearizeDepth(depth, input.near_far.x, input.near_far.y);

    // 0〜1に正規化して出力
    return linearDepth / input.near_far.y;
}