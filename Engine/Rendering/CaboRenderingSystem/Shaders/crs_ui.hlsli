#ifndef CRS_UI_HLSLI
#define CRS_UI_HLSLI

// UI(Image)用のShaderはこのファイルをincludeします。
// Materialのパラメーターは、メッシュ用と同じく space0 の cbuffer（名前自由）と t0..t7 です。
// Imageに設定したMaterialの_MainTexは t0 に入ります。
//
//   #include "../Engine/Rendering/CaboRenderingSystem/Shaders/crs_ui.hlsli"
//   UiVSOutput VSMain(UiVertexInput input) { return UiTransform(input); }
//   float4 PSMain(UiVSOutput input) : SV_TARGET { ... }

struct UiVertexInput
{
    float2 position : POSITION; // 画面の正規化座標（-1..1）
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

struct UiVSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

UiVSOutput UiTransform(UiVertexInput input)
{
    UiVSOutput output;
    output.position = float4(input.position, 0.0f, 1.0f);
    output.uv = input.uv;
    output.color = input.color;
    return output;
}

#endif
