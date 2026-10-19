#pragma once
#include "depth_texture.h"
#include "render_texture.h"

namespace engine
{
class RenderPipeline;

/// <summary>
/// 描画に使うCameraの情報(View / Projection、描画先など)です。
/// </summary>
struct Camera
{
    friend class RenderPipeline;

    UINT64 id;
    Color background_color;
    Matrix view;
    Matrix projection;
    std::shared_ptr<RenderTexture> render_texture;
    std::shared_ptr<DepthTexture> depth_texture;

    /// <summary>
    /// ViewMatrixの逆行列(CameraのWorldMatrix)を取得します。
    /// </summary>
    [[nodiscard]] Matrix GetWorldMatrix() const;

    /// <summary>
    /// 同じCameraであるかどうかをidで比較します。
    /// </summary>
    bool operator ==(const Camera &other) const
    {
        return this->id == other.id;
    }
};
}