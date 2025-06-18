#pragma once
#include <memory>
#include "../../Graphics/ConstantBuffer.h"
#include "../../Graphics/VertexBuffer.h"
#include "../../Graphics/IndexBuffer.h"
#include "../../Graphics/DescriptorHeap.h"
#include "../../Graphics/SharedStruct.h"

class PrimitiveShape
{
public:
    enum class ShapeType : uint8_t
    {
        Box,
        Cone,
        Cube,
        Cylinder,
        Dodecahedron,
        GeoSphere,
        IcosaHedron,
        Octahedron,
        Sphere,
        Teapot,
        Tetrahedron,
        Torus,
    };

    PrimitiveShape(ShapeType type);
    void Draw();

private:
    std::shared_ptr<CabotMesh> m_Mesh;
    std::shared_ptr<engine::VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<ConstantBuffer> m_WVPBuffer[RenderEngine::FRAME_BUFFER_COUNT];
    std::shared_ptr<DescriptorHandle> m_MaterialHandle;

    void CreateMesh(ShapeType type);
};