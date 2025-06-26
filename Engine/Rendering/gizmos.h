#pragma once
#include "event_receivers.h"
#include "CabotEngine/Graphics/VertexBuffer.h"
#include "Math/bounds.h"

#include <directxtk12/SimpleMath.h>

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

    static void Init();

public:
    static constexpr auto kDefaultColor = Color(1, 1, 1);

    int Order() override
    {
        return INT_MAX - 100; // at the very last
    }

    void OnDraw() override;

    static void DrawLine(const Vector3 &start, const Vector3 &end, const Color &color = kDefaultColor);
    static void DrawLines(const std::vector<Vector3> &line, const Color &color = kDefaultColor);
    static void DrawCircle(const Vector3 &center, float radius, const Color &color = kDefaultColor,
                           const Quaternion &rotation = Quaternion::Identity, int segments = 16);
};
}