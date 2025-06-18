#pragma once
#include "Engine.h"
#include "component.h"
#include "renderer.h"
#include "transform.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/rendering_serializer.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
class MeshRenderer : public Renderer
{
public:
    std::shared_ptr<Mesh> shared_mesh;
    std::vector<std::shared_ptr<Material>> shared_materials;

    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::vector<std::shared_ptr<IndexBuffer>> index_buffers;
    std::shared_ptr<ConstantBuffer> WVPBuffers[RenderEngine::FRAME_BUFFER_COUNT];
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