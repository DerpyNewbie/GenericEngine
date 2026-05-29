struct Particle {
    float2 position;
    float2 velocity;
};

// ★ここがミソ！ 描画側からは読み込み専用（SRV）として同じバッファを見る
StructuredBuffer<Particle> ParticleBuffer : register(t3);

struct VS_Output {
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

// 頂点バッファ（InputLayout）は使わず、頂点数だけを指定してDrawを呼ぶ
VS_Output vrt(uint vertexID : SV_VertexID)
{
    VS_Output output;
    
    // 1つの粒子につき4つの頂点を作る（0〜3で1つの四角形）
    uint particleIndex = vertexID / 4; // 何番目の粒子か (0〜127)
    uint vertexOffset   = vertexID % 4; // 四角形のどの角か (0〜3)
    
    // GPUバッファから中心座標を抜く
    float2 centerPos = ParticleBuffer[particleIndex].position;
    
    // 四角形の角のオフセット（ここで点の大きさを自由に変えられる！）
    // 0.05f を大きくすれば、点はいくらでも大きくなるよ！
    float size = 0.01f; 
    float2 offsets[4] = {
        float2(-size, -size), // 左下
        float2(-size,  size), // 左上
        float2( size, -size), // 右下
        float2( size,  size)  // 右上
    };
    
    // 中心座標にオフセットを足して、W=1.0で出力
    float2 finalPos = centerPos + offsets[vertexOffset];
    output.position = float4(finalPos, 0.0f, 1.0f);
    output.color    = float4(1.0f, 0.5f, 0.0f, 1.0f);
    
    return output;
}

float4 pix(VS_Output input) : SV_Target
{
    return input.color;
}