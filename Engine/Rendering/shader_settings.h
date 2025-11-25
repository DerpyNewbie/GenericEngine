#pragma once

struct ShaderSettings
{
    int z_test = 0;
    int z_write = 1;
    int cull = 0;

    int blend_src = 0;
    int blend_dst = 0;
    int blend_op = 0;

    int color_mask = 0;

    bool alpha_to_mask = false;
};

constexpr const char *ZTestNames[] = {
    "Less", "Greater", "LEqual", "GEqual", "Equal", "NotEqual", "Always", "Never",
};
constexpr const char *ZWriteNames[] = {
    "On", "Off",
};
constexpr const char *CullNames[] = {
    "Back", "Front", "Off",
};
constexpr const char *BlendFactorNames[] = {
    "One", "Zero", "SrcColor", "SrcAlpha", "DstColor", "DstAlpha",
    "OneMinusSrcColor", "OneMinusSrcAlpha",
    "OneMinusDstColor", "OneMinusDstAlpha"
};
constexpr const char *BlendOpNames[] = {
    "Add", "Subtract", "ReverseSubtract", "Min", "Max"
};
constexpr const char *ColorMaskNames[] = {
    "RGBA", "RGB", "A", "R", "G", "B", "0"
};
constexpr const char *LightModeNames[] = {
    "UniversalForward", "ShadowCaster", "DepthOnly", "Meta", "UniversalGBuffer"
};
constexpr const char *RenderTypeNames[] = {
    "Opaque", "Transparent", "TransparentCutout"
};
constexpr const char *RenderQueueNames[] = {
    "Background", "Geometry", "AlphaTest", "Transparent", "Overlay"
};

constexpr D3D12_COMPARISON_FUNC DX_ZTest[] = {
    D3D12_COMPARISON_FUNC_LESS,
    D3D12_COMPARISON_FUNC_GREATER,
    D3D12_COMPARISON_FUNC_LESS_EQUAL,
    D3D12_COMPARISON_FUNC_GREATER_EQUAL,
    D3D12_COMPARISON_FUNC_EQUAL,
    D3D12_COMPARISON_FUNC_NOT_EQUAL,
    D3D12_COMPARISON_FUNC_ALWAYS,
    D3D12_COMPARISON_FUNC_NEVER
};

constexpr D3D12_DEPTH_WRITE_MASK DX_ZWrite[] = {
    D3D12_DEPTH_WRITE_MASK_ALL,
    D3D12_DEPTH_WRITE_MASK_ZERO
};

constexpr D3D12_CULL_MODE DX_Cull[] = {
    D3D12_CULL_MODE_BACK,
    D3D12_CULL_MODE_FRONT,
    D3D12_CULL_MODE_NONE
};

constexpr D3D12_BLEND DX_BlendFactor[] = {
    D3D12_BLEND_ONE,
    D3D12_BLEND_ZERO,
    D3D12_BLEND_SRC_COLOR,
    D3D12_BLEND_SRC_ALPHA,
    D3D12_BLEND_DEST_COLOR,
    D3D12_BLEND_DEST_ALPHA,
    D3D12_BLEND_INV_SRC_COLOR,
    D3D12_BLEND_INV_SRC_ALPHA,
    D3D12_BLEND_INV_DEST_COLOR,
    D3D12_BLEND_INV_DEST_ALPHA
};

constexpr D3D12_BLEND_OP DX_BlendOp[] = {
    D3D12_BLEND_OP_ADD,
    D3D12_BLEND_OP_SUBTRACT,
    D3D12_BLEND_OP_REV_SUBTRACT,
    D3D12_BLEND_OP_MIN,
    D3D12_BLEND_OP_MAX
};

constexpr UINT8 DX_ColorMask[] = {
    D3D12_COLOR_WRITE_ENABLE_ALL,
    D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN | D3D12_COLOR_WRITE_ENABLE_BLUE,
    D3D12_COLOR_WRITE_ENABLE_ALPHA,
    D3D12_COLOR_WRITE_ENABLE_RED,
    D3D12_COLOR_WRITE_ENABLE_GREEN,
    D3D12_COLOR_WRITE_ENABLE_BLUE,
    0
};