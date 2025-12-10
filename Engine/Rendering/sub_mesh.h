#pragma once

namespace engine
{
struct SubMesh
{
    int base_vertex;
    int vertex_count;
    int base_index;
    int index_count;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(base_vertex),
            CEREAL_NVP(vertex_count),
            CEREAL_NVP(base_index),
            CEREAL_NVP(index_count)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::SubMesh, 1)