#pragma once
#include "component.h"

namespace engine
{
class FrameMetaData : public Component
{
public:
    Matrix bind_pose;
    Vector3 max_vert_pos;
    Vector3 min_vert_pos;
    Vector3 avg_vert_pos;
    int verts;
    int tris;
    int meshes;

    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this));
    }
};
}