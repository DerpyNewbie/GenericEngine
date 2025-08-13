#pragma once
#include "event_receivers.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/VertexBuffer.h"
#include "Components/renderer.h"

namespace engine
{
using namespace DirectX::SimpleMath;

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

    std::shared_ptr<VertexBuffer> m_vertex_buffers_[RenderEngine::FRAME_BUFFER_COUNT];
    int m_vertices_count_[RenderEngine::FRAME_BUFFER_COUNT];
    int m_last_back_buffer_idx_ = -1;

    static void Init();
    void CreateVertexBuffer(int current_back_buffer_idx);

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