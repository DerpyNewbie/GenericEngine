#pragma once
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
public:
    /// <summary>
    /// 線の頂点です。2つで1本の線になります。
    /// </summary>
    struct LineVertex
    {
        Vector3 position;
        Color color;
    };

private:
    friend class Engine;

    static std::vector<LineVertex> m_vertices_;

    static void Init();

public:
    static constexpr auto kDefaultColor = Color(1, 1, 1);

    /// <summary>
    /// 登録されている線の頂点を取り出し、リストを空にします。描画側(CrsBridge)が毎フレーム呼びます。
    /// </summary>
    static std::vector<LineVertex> TakeLines();

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