#pragma once
#include "component.h"
#include "renderer.h"
#include "transform.h"
#include "Rendering/MaterialData.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/rendering_serializer.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

typedef std::array<std::shared_ptr<DescriptorHandle>, engine::kParameterBufferType_Count> DescriptorHandlePerBuffer;
typedef std::array<DescriptorHandlePerBuffer, engine::kShaderType_Count> DescriptorHandlePerShader;

namespace engine
{
class MeshRenderer : public Renderer
{
public:
    std::shared_ptr<Mesh> shared_mesh;
    std::vector<std::shared_ptr<Material>> shared_materials;

    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::vector<std::shared_ptr<IndexBuffer>> index_buffers;
    std::vector<std::weak_ptr<MaterialData<std::vector<Matrix>>>> material_wvp_buffers[RenderEngine::FRAME_BUFFER_COUNT];
    std::vector<DescriptorHandlePerShader> material_handles;

    bool buffer_creation_failed = false;

    void OnInspectorGui() override;
    
    void OnDraw() override;

    virtual void ReconstructBuffers();
    virtual void UpdateBuffers();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(shared_mesh));
    }
};
}