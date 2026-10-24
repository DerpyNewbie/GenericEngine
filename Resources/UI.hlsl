// UI(Image)の標準のShader
// パラメーター: _MainTex（テクスチャ）, _Color（色）
#include "../Engine/Rendering/CaboRenderingSystem/Shaders/crs_ui.hlsli"

cbuffer Material
{
    float4 _Color;
};

Texture2D _MainTex : register(t0);
SamplerState _MainTex_Sampler : register(s0);

UiVSOutput VSMain(UiVertexInput input)
{
    return UiTransform(input);
}

float4 PSMain(UiVSOutput input) : SV_TARGET
{
    return _MainTex.Sample(_MainTex_Sampler, input.uv) * input.color * _Color;
}
