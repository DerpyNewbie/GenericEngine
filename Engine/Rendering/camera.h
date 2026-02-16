#pragma once
#include "depth_texture.h"
#include "render_texture.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class RenderPipeline;

struct Camera
{
    friend class RenderPipeline;

    Color background_color;
    Matrix view;
    Matrix projection;
    std::shared_ptr<RenderTexture> render_texture;
    std::shared_ptr<DepthTexture> depth_texture;

    [[nodiscard]] Matrix GetWorldMatrix() const;
};
}