#pragma once
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
/// <summary>
/// Simple line drawer for preview purposes 
/// </summary>
/// <remarks>
/// Can be called at any time. Rendering is guaranteed at the end of a frame. 
/// </remarks>
class Gizmos
{
    friend class Engine;

    static std::shared_ptr<Gizmos> m_instance_;
    static std::vector<Vertex> m_vertices_;
    static bool m_has_drawn_;

    std::shared_ptr<VertexBuffer> m_vertex_buffers_[2];
    std::shared_ptr<ConstantBuffer> m_constant_buffer_;
    std::shared_ptr<DescriptorHandle> m_desc_handle_;

    static void Init();
    static void ClearVertices();
    static void PreDrawCheck();

public:
    static constexpr auto kDefaultColor = Color(1, 1, 1);

    static void Render();

    static void DrawLine(const Vector3 &start, const Vector3 &end, const Color &color = kDefaultColor);
    static void DrawLines(const std::vector<Vector3> &line, const Color &color = kDefaultColor);
    static void DrawCircle(const Vector3 &center, float radius, const Color &color = kDefaultColor,
                           const Quaternion &rotation = Quaternion::Identity, int segments = 16);
    static void DrawSphere(const Vector3 &center, float radius, const Color &color = kDefaultColor, int segments = 16);
    static void DrawBounds(const DirectX::BoundingBox &bounds, const Color &color = kDefaultColor,
                           const Matrix &mat = DirectX::XMMatrixIdentity());
};
}