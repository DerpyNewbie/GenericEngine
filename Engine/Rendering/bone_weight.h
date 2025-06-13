#pragma once
#include <vector>

namespace engine
{
struct BoneWeight
{
    std::vector<int> bone_indices;
    std::vector<float> weights;
};
}