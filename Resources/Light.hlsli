#define SHADOW_CASCADE_COUNT 3

struct Light
{
    int type;
    int cast_shadow;
    float intensity;
    float range;

    float4 pos;
    float4 direction;
    float4 color;

    float inner_cos;
    float outer_cos;
    float2 padding;
};

cbuffer ShadowCascadeSprits : register(b2)
{
    float cascade_sprits[SHADOW_CASCADE_COUNT];
}

cbuffer LightCount : register (b3)
{
    int light_count;
    int a[63];
}

StructuredBuffer<float4x4> LightViewProj : register(t1);
StructuredBuffer<Light> Lights : register (t2);
Texture2DArray ShadowMaps : register (t3);
Texture2D _MainTex : register (t4);
SamplerState smp : register (s0);
SamplerComparisonState shadowSampler : register (s1);

float SampleShadowPCF(float3 shadowCoord, int lightIndex)
{
    float shadow = 0.0;
    const float2 texelSize = 1.0 / float2(1920, 1065);

    shadowCoord.z -= 0.002f;

    [unroll]
    for (int x = -1; x <= 1; x++)
    {
        [unroll]
        for (int y = -1; y <= 1; y++)
        {
            float2 offset = float2(x, y) * texelSize;
            shadow += ShadowMaps.SampleCmpLevelZero(
                shadowSampler,
                float3(shadowCoord.xy + offset, lightIndex),
                shadowCoord.z
                );
        }
    }

    shadow /= 9.0;
    return shadow;
}

float CalcShadow(float3 world_pos, int shadow_map_index)
{
    float4 worldPos = float4(world_pos, 1);
    float4 lightClip = mul(LightViewProj[shadow_map_index], worldPos);
    float3 shadowCoord;
    shadowCoord.xy = lightClip.xy / lightClip.w * 0.5f + 0.5f;
    shadowCoord.y = 1 - shadowCoord.y;
    shadowCoord.z = lightClip.z / lightClip.w;

    float shadow = 0;
    if (shadowCoord.x < 0 || shadowCoord.x > 1 ||
        shadowCoord.y < 0 || shadowCoord.y > 1 || shadowCoord.z >= 1.0f)
    {
        shadow = 1.0f;
    }
    else
    {
        shadow = SampleShadowPCF(shadowCoord, shadow_map_index);
    }

    return shadow;
}

float3 CalcDirectionalShadow(Light light, float3 normal, float3 world_pos, int shadow_map_index)
{
    float3 L = normalize(-light.direction);
    float NdotL = saturate(dot(normal, L));
    float3 brightness = 0;

    if (light.cast_shadow == 0)
    {
        brightness += NdotL * light.color.rgb * light.intensity;
        return brightness;
    }

    float shadow = CalcShadow(world_pos, shadow_map_index);
    brightness += shadow * NdotL * light.color.rgb * light.intensity;
    return brightness;
}

float3 CalcSpotShadow(Light light, float3 normal, float3 world_pos, int shadow_map_index)
{
    float3 L = light.pos - world_pos;
    float dist = length(L);
    L /= dist;

    float spotFactor = dot(L, -light.direction);
    float outer = light.outer_cos;
    float inner = light.inner_cos;
    float smoothEdge = saturate((spotFactor - outer) / (inner - outer));

    float attenuation = saturate(1.0 - dist / light.range);
    float3 lighting = smoothEdge * attenuation * max(dot(normal, L), 0) * light.color * light.intensity;
    if (light.cast_shadow == 0)
        return lighting;

    float shadow = CalcShadow(world_pos, shadow_map_index);
    return lighting * shadow;
}