#pragma once

namespace engine
{
/// <summary>
/// 頂点に影響するBoneのindexとWeightです。
/// </summary>
struct BoneWeight
{
    int bone_index;
    float weight;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(bone_index),
            CEREAL_NVP(weight)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::BoneWeight, 1)