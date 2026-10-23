// デバッグ表示用のShader
// パラメーター: _DebugType（0: UV, 1: 法線, 2: ボーンの重み）, _WeightIndex（_DebugType 2の時に表示する重みの番号 0..3）
#include "../Engine/Rendering/CaboRenderingSystem/Shaders/crs_common.hlsli"

cbuffer Material
{
    int _DebugType;
    int _WeightIndex;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VSMain(StandardVertexInput input)
{
    VSOutput output;
    float3 world_normal;
    output.position = mul(view_projection, TransformToWorld(input, world_normal));

    switch (_DebugType)
    {
    case 0:
        output.color = float4(input.uv, 0.0f, 1.0f);
        break;
    case 1:
        output.color = float4(world_normal * 0.5f + 0.5f, 1.0f);
        break;
    case 2:
        output.color = float4(input.bone_weight[clamp(_WeightIndex, 0, 3)], 0.0f, 0.0f, 1.0f);
        break;
    default:
        output.color = float4(1, 0, 1, 1);
        break;
    }
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return input.color;
}
