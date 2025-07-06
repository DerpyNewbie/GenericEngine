#pragma once
#include "sub_mesh.h"
#include "bone_weight.h"

namespace cereal
{
template <class Archive>
void serialize(Archive &archive, engine::SubMesh &sub_mesh)
{
    archive(CEREAL_NVP(sub_mesh.base_index), CEREAL_NVP(sub_mesh.base_vertex),
            CEREAL_NVP(sub_mesh.index_count), CEREAL_NVP(sub_mesh.vertex_count));
}
template <class Archive>
void serialize(Archive &archive, engine::BoneWeight &bone_weight)
{
    archive(CEREAL_NVP(bone_weight.bone_index), CEREAL_NVP(bone_weight.weight));
}
}