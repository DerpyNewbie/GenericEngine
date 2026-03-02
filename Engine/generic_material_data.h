#pragma once

namespace engine
{
struct alignas(16) GenericMaterialData
{
    Color base_color = Color(1, 1, 1, 1);

    float roughness;
    float fresnel;
    float metallic;
    float alpha_cut_off;

    Color emissive_color;

    int albedo_map_idx;
    int normal_map_idx;
    int metallic_roughness_map_idx;
    int emissive_map_idx;

    DirectX::XMFLOAT4 custom_params[4];

    uint32_t flags;
    uint32_t material_type;
    float padding[2];

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(base_color),
            CEREAL_NVP(roughness),
            CEREAL_NVP(fresnel),
            CEREAL_NVP(alpha_cut_off),
            CEREAL_NVP(emissive_color),
            CEREAL_NVP(albedo_map_idx),
            CEREAL_NVP(normal_map_idx),
            CEREAL_NVP(metallic_roughness_map_idx),
            CEREAL_NVP(emissive_map_idx),
            CEREAL_NVP(custom_params),
            CEREAL_NVP(flags),
            CEREAL_NVP(material_type)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::GenericMaterialData, 0)