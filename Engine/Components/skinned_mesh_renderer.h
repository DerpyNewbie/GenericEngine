#pragma once
#include "DxLib.h"
#include "game_object.h"
#include "renderer.h"
#include "Rendering/texture2d.h"
#include "Rendering/mesh.h"
#include "Rendering/rendering_serializer.h"

#include <vector>

namespace engine
{
class SkinnedMeshRenderer : public Renderer
{
    bool m_draw_bones_ = false;
    Texture2D m_texture_;

public:
    std::shared_ptr<Mesh> shared_mesh;
    std::vector<std::weak_ptr<Transform>> transforms;
    int vertex_buffer_handle = -1;
    int index_buffer_handle = -1;

    void OnInspectorGui() override;

    void OnDraw() override;

    void ReconstructBuffers();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(shared_mesh), CEREAL_NVP(transforms));
    }
};
}