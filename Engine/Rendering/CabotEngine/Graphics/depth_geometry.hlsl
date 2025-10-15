#define MAX_SHADOWMAP_COUNT 10

StructuredBuffer<float4x4> LightViewProj : register(t1);

cbuffer LightCount : register (b3)
{
    int light_count;
}

struct VSOutput
{
    float3 world_pos : WORLDPOS;
};

struct GSOutput
{
    float4 pos : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(3 * MAX_SHADOWMAP_COUNT)]
void geo(triangle VSOutput input[3], inout TriangleStream<GSOutput> tri_stream)
{
    for (int i = 0; i < MAX_SHADOWMAP_COUNT; ++i)
    {
        float4x4 viewproj = LightViewProj[i];
        for (uint j = 0; j < 3; j++)
        {
            GSOutput element;
            element.pos = mul(viewproj, float4(input[j].world_pos, 1));
            element.RTIndex = i;
            tri_stream.Append(element);
        }
        tri_stream.RestartStrip();
    }
}