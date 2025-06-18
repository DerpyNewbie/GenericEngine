#pragma once
#include "DxLib.h"
#include "game_object.h"
#include "mesh_renderer.h"

#include <vector>

namespace engine
{
class SkinnedMeshRenderer : public MeshRenderer
{
    bool m_draw_bones_ = false;

public:
    constexpr static int kLimitBonesPerVertex = 4;
    std::vector<std::weak_ptr<Transform>> transforms;

    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<MeshRenderer>(this), CEREAL_NVP(shared_mesh), CEREAL_NVP(transforms));
    }
};
}