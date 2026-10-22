#include "Rendering/CaboRenderingSystem/Pipeline/shadow_system.h"

#include <algorithm>
#include <cmath>

namespace CRS
{
namespace
{
void NearFar(const Matrix &projection, float &out_near, float &out_far)
{
    // 右手系の透視投影 / 正射影の行列から、near / farを取り出す
    if (projection._44 == 1.0f)
    {
        out_near = projection._43 / projection._33;
        out_far = (projection._43 - 1.0f) / projection._33;
    }
    else
    {
        out_near = projection._43 / projection._33;
        out_far = projection._43 / (projection._33 + 1.0f);
    }
}

std::array<Vector3, 8> FrustumCorners(const Matrix &view_projection)
{
    // 0..3: near, 4..7: far
    constexpr Vector3 kNdcCorners[8] = {
        {-1, -1, 0}, {-1, 1, 0}, {1, 1, 0}, {1, -1, 0},
        {-1, -1, 1}, {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}
    };

    const auto inverse = view_projection.Invert();
    std::array<Vector3, 8> corners;
    for (int i = 0; i < 8; ++i)
        corners[i] = Vector3::Transform(kNdcCorners[i], inverse);
    return corners;
}

Vector3 SafeUp(const Vector3 &direction)
{
    return std::fabs(direction.Dot(Vector3::Up)) > 0.9f ? Vector3::Forward : Vector3::Up;
}

RenderView MakeShadowView(const Matrix &view, const Matrix &projection, const TextureHandle shadow_maps,
                          const uint32_t slice, const uint32_t map_size)
{
    RenderView shadow_view;
    shadow_view.kind = RenderViewKind::Shadow;
    shadow_view.view = view;
    shadow_view.projection = projection;
    shadow_view.view_projection = view * projection;
    shadow_view.previous_view_projection = shadow_view.view_projection;
    shadow_view.position = view.Invert().Translation();
    shadow_view.viewport = {0, 0, static_cast<float>(map_size), static_cast<float>(map_size), 0, 1};
    shadow_view.scissor = {0, 0, static_cast<int32_t>(map_size), static_cast<int32_t>(map_size)};
    shadow_view.depth_target = shadow_maps;
    shadow_view.depth_slice = slice;
    shadow_view.depth_format = Format::D32_Float;
    shadow_view.depth_load = LoadOp::Clear;
    shadow_view.depth_only = true;
    shadow_view.frustum_cull = false;
    return shadow_view;
}
}

ShadowSystem::ShadowSystem(IRenderDevice &device, const uint32_t map_size, const uint32_t max_maps) :
    m_device_(device), m_map_size_(map_size), m_max_maps_(max_maps)
{
    TextureDesc desc;
    desc.width = map_size;
    desc.height = map_size;
    desc.array_size = max_maps;
    desc.dimension = TextureDimension::Texture2DArray;
    desc.format = Format::D32_Float;
    desc.usage = TextureUsage::DepthStencil | TextureUsage::ShaderResource;
    desc.debug_name = "CRS ShadowMaps";
    m_shadow_maps_ = Texture(device, desc);
}

void ShadowSystem::BeginFrame()
{
    m_next_buffer_set_ = 0;
}

FrameLighting ShadowSystem::Prepare(const FrameSnapshot &frame, const RenderView &camera_view,
                                    std::vector<RenderView> &out_shadow_views)
{
    if (m_next_buffer_set_ >= m_buffer_sets_.size())
    {
        m_buffer_sets_.push_back({
            StructuredBuffer<GpuLight>(m_device_, kMaxLights, MemoryType::Upload, BufferUsage::None, "CRS Lights"),
            StructuredBuffer<Matrix>(m_device_, m_max_maps_, MemoryType::Upload, BufferUsage::None,
                                     "CRS LightViewProjections")
        });
    }
    auto &buffers = m_buffer_sets_[m_next_buffer_set_++];

    float camera_near, camera_far;
    NearFar(camera_view.projection, camera_near, camera_far);
    const auto camera_corners = FrustumCorners(camera_view.view_projection);
    const auto &splits = frame.environment.cascade_splits;

    std::vector<GpuLight> gpu_lights;
    std::vector<Matrix> light_view_projections;
    gpu_lights.reserve(std::min<size_t>(frame.lights.size(), kMaxLights));

    for (const auto &light : frame.lights)
    {
        if (gpu_lights.size() >= kMaxLights)
            break;

        GpuLight gpu_light;
        gpu_light.type = static_cast<int32_t>(light.type);
        gpu_light.intensity = light.intensity;
        gpu_light.range = light.range;
        gpu_light.position = light.position;
        gpu_light.direction = light.direction;
        gpu_light.direction.Normalize();
        gpu_light.inner_cos = light.inner_cos;
        gpu_light.outer_cos = light.outer_cos;
        gpu_light.color = light.color;

        const uint32_t required_maps = light.type == LightType::Directional ? kShadowCascadeCount : 1;
        const bool can_cast_shadow = light.cast_shadow &&
                                     light_view_projections.size() + required_maps <= m_max_maps_;

        if (can_cast_shadow)
        {
            gpu_light.cast_shadow = 1;
            gpu_light.shadow_index = static_cast<int32_t>(light_view_projections.size());

            if (light.type == LightType::Directional)
            {
                for (uint32_t cascade = 0; cascade < kShadowCascadeCount; ++cascade)
                {
                    // カメラの視錐台を、カスケードの範囲で切り出す
                    const float cascade_near = cascade == 0 ? camera_near : splits[cascade - 1];
                    const float cascade_far = std::min(splits[cascade], camera_far);
                    const float t_near = (cascade_near - camera_near) / (camera_far - camera_near);
                    const float t_far = (cascade_far - camera_near) / (camera_far - camera_near);

                    std::array<Vector3, 8> corners;
                    for (int i = 0; i < 4; ++i)
                    {
                        corners[i] = Vector3::Lerp(camera_corners[i], camera_corners[i + 4], t_near);
                        corners[i + 4] = Vector3::Lerp(camera_corners[i], camera_corners[i + 4], t_far);
                    }

                    // 切り出した視錐台を包む球を、ライトの向きから見る正射影で覆う
                    Vector3 center;
                    for (const auto &corner : corners)
                        center += corner;
                    center /= 8.0f;

                    float radius = 0.0f;
                    for (const auto &corner : corners)
                        radius = std::max(radius, (corner - center).Length());
                    radius = std::ceil(radius);

                    // 視錐台の外から影を落とすものも入るよう、ライトの位置を後ろに下げる
                    constexpr float kCasterMargin = 50.0f;
                    const auto eye = center - gpu_light.direction * (radius + kCasterMargin);
                    const auto light_view = Matrix::CreateLookAt(eye, center, SafeUp(gpu_light.direction));
                    const auto light_projection = Matrix::CreateOrthographic(radius * 2, radius * 2, 0.01f,
                                                                             radius * 2 + kCasterMargin);

                    const auto slice = static_cast<uint32_t>(light_view_projections.size());
                    light_view_projections.push_back(light_view * light_projection);
                    out_shadow_views.push_back(MakeShadowView(light_view, light_projection,
                                                              m_shadow_maps_.GetHandle(), slice, m_map_size_));
                }
            }
            else
            {
                const auto light_view = Matrix::CreateLookAt(light.position, light.position + gpu_light.direction,
                                                             SafeUp(gpu_light.direction));
                const float outer_angle = std::acos(std::clamp(light.outer_cos, -1.0f, 1.0f));
                const auto light_projection = Matrix::CreatePerspectiveFieldOfView(
                    std::clamp(outer_angle * 2.0f, 0.1f, 3.0f), 1.0f, 0.1f, std::max(light.range, 0.2f));

                const auto slice = static_cast<uint32_t>(light_view_projections.size());
                light_view_projections.push_back(light_view * light_projection);
                out_shadow_views.push_back(MakeShadowView(light_view, light_projection, m_shadow_maps_.GetHandle(),
                                                          slice, m_map_size_));
            }
        }

        gpu_lights.push_back(gpu_light);
    }

    if (!gpu_lights.empty())
        buffers.lights.Set(gpu_lights);
    if (!light_view_projections.empty())
        buffers.light_view_projections.Set(light_view_projections);

    FrameLighting lighting;
    lighting.constants.light_count = static_cast<uint32_t>(gpu_lights.size());
    lighting.constants.shadow_map_count = static_cast<uint32_t>(light_view_projections.size());
    lighting.constants.shadow_map_size = Vector2(static_cast<float>(m_map_size_), static_cast<float>(m_map_size_));
    lighting.constants.cascade_splits = Vector4(splits[0], splits[1], splits[2], 0);
    lighting.constants.ambient = frame.environment.ambient;
    lighting.lights = buffers.lights.GetHandle();
    lighting.light_view_projections = buffers.light_view_projections.GetHandle();
    lighting.shadow_maps = m_shadow_maps_.GetHandle();
    return lighting;
}
}