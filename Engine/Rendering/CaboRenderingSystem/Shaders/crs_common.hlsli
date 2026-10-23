#ifndef CRS_COMMON_HLSLI
#define CRS_COMMON_HLSLI

// 共通ルートシグネチャの割り当て
//   space1 (エンジン): b0 View / b1 Object / b2 Lighting
//   space0 (マテリアル): cbuffer は名前自由で b0..b3 / t0..t7 テクスチャ / s0..s3 サンプラー。registerは省略してよい
//   t0..t7 (space1): ライト、影、ボーン / s0, s1 (space1): 固定のサンプラー
// 行列はC++側(SimpleMath)の並びのまま渡しているので、mul(matrix, vector)の順で掛けます。

#define SHADOW_CASCADE_COUNT 3
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_SPOT 1
#define OBJECT_FLAG_SKINNED 1

cbuffer View : register(b0, space1)
{
    float4x4 view;
    float4x4 projection;
    float4x4 view_projection;
    float4x4 previous_view_projection;
    float4 camera_position;
    float4 time; // x: 起動からの時間, y: 前フレームからの時間
};

cbuffer Object : register(b1, space1)
{
    float4x4 world;
    float4x4 previous_world;
    uint object_flags;
};

cbuffer Lighting : register(b2, space1)
{
    uint light_count;
    uint shadow_map_count;
    float2 shadow_map_size;
    float4 cascade_splits; // xyz: カスケードを切り替えるカメラからの距離
    float4 ambient_color;
};

struct GpuLight
{
    int type;
    int cast_shadow;
    float intensity;
    float range;
    float3 position;
    int shadow_index; // 影の行列の先頭。平行光源はカスケード分だけ続く
    float3 direction;
    float inner_cos;
    float4 color;
    float outer_cos;
    float3 padding;
};

StructuredBuffer<GpuLight> lights : register(t0, space1);
StructuredBuffer<float4x4> light_view_projections : register(t1, space1);
Texture2DArray shadow_maps : register(t2, space1);
StructuredBuffer<float4x4> bone_matrices : register(t3, space1);

SamplerComparisonState shadow_sampler : register(s0, space1);
SamplerState linear_clamp_sampler : register(s1, space1);

// 頂点の入力。エンジンのVertex構造体のうち、使うものだけを宣言します
struct StandardVertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    uint bones_per_vertex : BONESPERVERTEX;
    uint4 bone_index : BONEINDEX;
    float4 bone_weight : BONEWEIGHT;
};

// 頂点変形（スキニングなど）は全パスのシェーダーでここを共有します
void ApplySkinning(StandardVertexInput input, inout float4 position, inout float3 normal)
{
    if ((object_flags & OBJECT_FLAG_SKINNED) == 0 || input.bones_per_vertex == 0)
        return;

    float4 skinned_position = 0;
    float3 skinned_normal = 0;

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        const float weight = input.bone_weight[i];
        if (weight > 0)
        {
            const float4x4 bone = bone_matrices[input.bone_index[i]];
            skinned_position += mul(bone, position) * weight;
            skinned_normal += mul((float3x3)bone, normal) * weight;
        }
    }

    position = skinned_position;
    normal = skinned_normal;
}

float4 TransformToWorld(StandardVertexInput input, out float3 world_normal)
{
    float4 position = float4(input.position, 1.0f);
    float3 normal = input.normal;
    ApplySkinning(input, position, normal);

    world_normal = normalize(mul((float3x3)world, normal));
    return mul(world, position);
}

// ---- ライティング ----

float SampleShadow(float3 world_position, int shadow_index)
{
    const float4 light_clip = mul(light_view_projections[shadow_index], float4(world_position, 1.0f));
    float3 coord;
    coord.xy = light_clip.xy / light_clip.w * 0.5f + 0.5f;
    coord.y = 1.0f - coord.y;
    coord.z = light_clip.z / light_clip.w;

    if (coord.x < 0 || coord.x > 1 || coord.y < 0 || coord.y > 1 || coord.z >= 1.0f)
        return 1.0f;

    coord.z -= 0.003f;

    // 3x3のPCF
    const float2 texel = 1.0f / shadow_map_size;
    float shadow = 0;
    [unroll]
    for (int x = -1; x <= 1; ++x)
    {
        [unroll]
        for (int y = -1; y <= 1; ++y)
        {
            shadow += shadow_maps.SampleCmpLevelZero(shadow_sampler, float3(coord.xy + float2(x, y) * texel, shadow_index),
                                                     coord.z);
        }
    }
    return shadow / 9.0f;
}

int SelectCascade(float view_depth)
{
    int cascade = SHADOW_CASCADE_COUNT - 1;
    [unroll]
    for (int i = SHADOW_CASCADE_COUNT - 1; i >= 0; --i)
    {
        if (view_depth < cascade_splits[i])
            cascade = i;
    }
    return cascade;
}

float3 CalcLighting(float3 normal, float3 world_position)
{
    if (light_count == 0)
        return ambient_color.rgb + 0.75f;

    const float view_depth = abs(mul(view, float4(world_position, 1.0f)).z);

    float3 result = ambient_color.rgb;
    for (uint i = 0; i < light_count; ++i)
    {
        const GpuLight light = lights[i];
        float3 radiance = 0;
        float shadow = 1.0f;

        if (light.type == LIGHT_TYPE_DIRECTIONAL)
        {
            const float3 l = normalize(-light.direction);
            radiance = saturate(dot(normal, l)) * light.color.rgb * light.intensity;
            if (light.cast_shadow != 0)
                shadow = SampleShadow(world_position, light.shadow_index + SelectCascade(view_depth));
        }
        else if (light.type == LIGHT_TYPE_SPOT)
        {
            float3 l = light.position - world_position;
            const float distance = length(l);
            l /= distance;

            const float spot = dot(l, -light.direction);
            const float edge = saturate((spot - light.outer_cos) / max(light.inner_cos - light.outer_cos, 0.0001f));
            const float attenuation = saturate(1.0f - distance / light.range);
            radiance = edge * attenuation * saturate(dot(normal, l)) * light.color.rgb * light.intensity;
            if (light.cast_shadow != 0)
                shadow = SampleShadow(world_position, light.shadow_index);
        }

        result += radiance * shadow;
    }
    return result;
}

#endif
