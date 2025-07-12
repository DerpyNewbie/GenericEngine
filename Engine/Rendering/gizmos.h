#pragma once
#include "event_receivers.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
using namespace DirectX::SimpleMath;

/// <summary>
/// Simple line drawer for preview purposes 
/// </summary>
/// <remarks>
/// Can be called at any time. Rendering is guaranteed at the end of a frame. 
/// </remarks>
class Gizmos : public IDrawCallReceiver
{
    friend class Engine;

    static std::shared_ptr<Gizmos> m_instance_;
    static std::vector<Vertex> m_vertices_;

    std::shared_ptr<VertexBuffer> m_vertex_buffers_[2];
    std::shared_ptr<ConstantBuffer> m_constant_buffer_;
    std::shared_ptr<DescriptorHandle> m_desc_handle_;

    static void Init();

public:
    static constexpr auto kDefaultColor = Color(1, 1, 1);

    int Order() override
    {
        return INT_MAX - 1000; // at the very last
    }

    void OnDraw() override;

    static void DrawLine(const Vector3 &start, const Vector3 &end, const Color &color = kDefaultColor);
    static void DrawLines(const std::vector<Vector3> &line, const Color &color = kDefaultColor);
    static void DrawCircle(const Vector3 &center, float radius, const Color &color = kDefaultColor,
                           const Quaternion &rotation = Quaternion::Identity, int segments = 16);
    static void DrawSphere(const Vector3 &center, float radius, const Color &color = kDefaultColor, int segments = 16);
};
}