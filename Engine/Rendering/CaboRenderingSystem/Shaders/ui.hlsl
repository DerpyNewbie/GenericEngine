#include "crs_ui.hlsli"

// Materialを使わないUI（テキスト、Shaderの無いImage）用の標準のShader
Texture2D _MainTex : register(t0);
SamplerState ui_sampler : register(s0);

UiVSOutput VSMain(UiVertexInput input)
{
    return UiTransform(input);
}

float4 PSMain(UiVSOutput input) : SV_TARGET
{
    return _MainTex.Sample(ui_sampler, input.uv) * input.color;
}
