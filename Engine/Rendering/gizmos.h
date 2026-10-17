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

    std::shared_ptr<VertexBuffer> m_vertex_buffers_[RenderEngine::kFrame_Buffer_Count];
    std::shared_ptr<Shader> m_line_shader_;
    int m_vertices_count_[RenderEngine::kFrame_Buffer_Count];
    int m_last_back_buffer_idx_ = -1;

    /// <summary>
    /// Gizmosのインスタンスを生成します。
    /// </summary>
    static void Init();
    /// <summary>
    /// 登録されている線の頂点からVertexBufferを作成し、頂点のリストをクリアします。
    /// </summary>
    /// <param name="current_back_buffer_idx">現在のBackBufferのindex</param>
    void CreateVertexBuffer(int current_back_buffer_idx);

public:
    static constexpr auto kDefaultColor = Color(1, 1, 1);

    /// <summary>
    /// 登録されている線を描画します。
    /// </summary>
    static void Render();

    /// <summary>
    /// 線を登録します。実際の描画はRenderで行われます。
    /// </summary>
    /// <param name="start">始点</param>
    /// <param name="end">終点</param>
    /// <param name="color">線の色</param>
    static void DrawLine(const Vector3 &start, const Vector3 &end, const Color &color = kDefaultColor);
    /// <summary>
    /// 点を順番に結ぶ線を登録します。
    /// </summary>
    /// <param name="line">結ぶ点のリスト</param>
    /// <param name="color">線の色</param>
    static void DrawLines(const std::vector<Vector3> &line, const Color &color = kDefaultColor);
    /// <summary>
    /// 円を登録します。
    /// </summary>
    /// <param name="center">中心</param>
    /// <param name="radius">半径</param>
    /// <param name="color">線の色</param>
    /// <param name="rotation">円の向き</param>
    /// <param name="segments">円の分割数</param>
    static void DrawCircle(const Vector3 &center, float radius, const Color &color = kDefaultColor,
                           const Quaternion &rotation = Quaternion::Identity, int segments = 16);
    /// <summary>
    /// 向きの異なる複数の円を組み合わせて、球を登録します。
    /// </summary>
    /// <param name="center">中心</param>
    /// <param name="radius">半径</param>
    /// <param name="color">線の色</param>
    /// <param name="segments">分割数</param>
    static void DrawSphere(const Vector3 &center, float radius, const Color &color = kDefaultColor, int segments = 16);
    /// <summary>
    /// BoundingBoxの12本の辺を登録します。
    /// </summary>
    /// <param name="bounds">描画するBoundingBox</param>
    /// <param name="color">線の色</param>
    /// <param name="mat">boundsの各頂点に適用するMatrix</param>
    static void DrawBounds(const DirectX::BoundingBox &bounds, const Color &color = kDefaultColor,
                           const Matrix &mat = DirectX::XMMatrixIdentity());
};
}