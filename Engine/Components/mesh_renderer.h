#pragma once
#include "component.h"
#include "renderer.h"
#include "transform.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/rendering_serializer.h"

namespace engine
{
class MeshRenderer : public Renderer
{
public:
    std::shared_ptr<Mesh> shared_mesh;
    std::vector<std::shared_ptr<Material>> shared_materials;

    int vertex_buffer_handle;
    std::vector<int> index_buffer_handles;
    std::vector<int> texture_handles;

    bool buffer_creation_failed = false;

    void OnInspectorGui() override;

    void OnDraw() override;

    void ReconstructBuffers();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(shared_mesh));
    }
};
}