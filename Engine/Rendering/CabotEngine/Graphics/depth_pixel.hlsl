struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 near_far : NEARFAR;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

float LinearizeDepth(float depth, float nearZ, float farZ)
{
    return (nearZ * farZ) / (farZ - depth * (farZ - nearZ));
}

float pix(GSOutput input) : SV_TARGET
{
    float depth = input.pos.z / input.pos.w;
    
    float linearDepth = LinearizeDepth(depth, input.near_far.x, input.near_far.y);
    
    return linearDepth / input.near_far.y;
}