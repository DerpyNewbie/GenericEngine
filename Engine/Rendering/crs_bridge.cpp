#include "pch.h"
#include "crs_bridge.h"

#include "application.h"
#include "depth_texture.h"
#include "game_object.h"
#include "gizmos.h"
#include "lighting.h"
#include "render_texture.h"
#include "skybox.h"
#include "vertex.h"
#include "Components/camera_component.h"
#include "engine_time.h"
#include "CaboRenderingSystem/render_engine.h"
#include "CaboRenderingSystem/RHI/gpu_resources.h"
#include "CaboRenderingSystem/RHI/D3D12/d3d12_render_device.h"

namespace engine
{
namespace
{
/// <summary>
/// RenderTextureに描くCameraの深度バッファです（DepthTextureが設定されていない場合に使います）。
/// </summary>
struct CameraDepthEntry
{
    CRS::Texture depth;
    uint32_t width = 0;
    uint32_t height = 0;
};

std::unordered_map<const CameraComponent *, CameraDepthEntry> g_camera_depths;
std::vector<CRS::LightId> g_lights;
CRS::CameraId g_fallback_camera;
bool g_main_camera_submitted = false;

// 終了時の静的変数の破棄順で問題が起きないよう、解放しない（プロセスの終了で解放される）
CRS::RenderEngine *g_crs = nullptr;
CRS::D3D12RenderDevice *g_device = nullptr;

Vector2 WindowSize()
{
    return Vector2(static_cast<float>(Application::WindowWidth()), static_cast<float>(Application::WindowHeight()));
}

CRS::TextureHandle GetCameraDepth(const CameraComponent &camera, const uint32_t width, const uint32_t height)
{
    if (const auto depth_texture = camera.TargetDepthTexture())
        return depth_texture->GpuTexture();

    auto &entry = g_camera_depths[&camera];
    if (entry.depth.IsValid() && entry.width == width && entry.height == height)
        return entry.depth.GetHandle();

    CRS::TextureDesc desc;
    desc.width = width;
    desc.height = height;
    desc.format = CRS::Format::D32_Float;
    desc.usage = CRS::TextureUsage::DepthStencil;
    desc.debug_name = "CameraDepth";
    entry.depth = CRS::Texture(CrsBridge::Crs().Device(), desc);
    entry.width = width;
    entry.height = height;
    return entry.depth.GetHandle();
}

/// <summary>
/// 画面のピクセル座標を、正規化座標(-1..1)にします。
/// </summary>
Vector2 PixelToNdc(const Vector2 &pixel, const Vector2 &screen_size)
{
    return Vector2(pixel.x / screen_size.x * 2.0f - 1.0f, 1.0f - pixel.y / screen_size.y * 2.0f);
}

void SubmitGizmoLines()
{
    const auto lines = Gizmos::TakeLines();
    auto &immediate = CrsBridge::Crs().Immediate();
    for (size_t i = 0; i + 1 < lines.size(); i += 2)
        immediate.AddLine(lines[i].position, lines[i + 1].position, lines[i].color);
}

void SubmitLights()
{
    auto &store = CrsBridge::Crs().Lights();
    for (const auto id : g_lights)
        store.Unregister(id);
    g_lights.clear();

    for (const auto &light : Lighting::Instance()->Lights())
    {
        light->UpdateData();
        const auto &data = light->Data();

        CRS::LightData crs_light;
        crs_light.type = data.type == static_cast<int>(kLightType::kSpotLight)
                             ? CRS::LightType::Spot
                             : CRS::LightType::Directional;
        crs_light.color = data.color;
        crs_light.intensity = data.intensity;
        crs_light.range = data.range;
        crs_light.position = data.pos;
        crs_light.direction = data.direction;
        crs_light.inner_cos = data.inner_cos;
        crs_light.outer_cos = data.outer_cos;
        crs_light.cast_shadow = data.cast_shadow != 0;
        g_lights.push_back(store.Register(crs_light));
    }
}

void SubmitEnvironment()
{
    auto &environment = CrsBridge::Crs().Environment();

    const auto texture_cube = Skybox::Instance()->TextureCube();
    environment.skybox = texture_cube != nullptr ? texture_cube->GpuTexture() : CRS::TextureHandle{};

    const auto &slices = Lighting::Instance()->CascadeSlices();
    for (size_t i = 0; i < environment.cascade_splits.size() && i < slices.size(); ++i)
        environment.cascade_splits[i] = slices[i];
}

/// <summary>
/// Main Cameraが無い場合に、原点から前方を向いた既定の視点で画面に描きます。
/// </summary>
void SubmitFallbackCamera()
{
    auto &crs = CrsBridge::Crs();
    if (crs.Cameras().GetCommon(g_fallback_camera) == nullptr)
        g_fallback_camera = crs.CreateCamera();

    CRS::CameraCommon common;
    if (!g_main_camera_submitted)
    {
        const auto window_size = WindowSize();
        common.view = Matrix::CreateLookAt(Vector3::Zero, Vector3::Forward, Vector3::Up);
        common.projection = Matrix::CreatePerspectiveFieldOfView(
            75 * Mathf::kDeg2Rad, Application::WindowAspectRatio(), 0.1f, 1000.0f);
        common.viewport = {0, 0, window_size.x, window_size.y, 0, 1};
        common.color_target = CrsBridge::Device().GetBackBuffer();
        common.depth_target = crs.MainDepth();
        common.clear_color = {0, 0, 0, 1};
        common.draw_overlays = true;
        common.priority = 100;
    }
    // Main Cameraがある場合は、描画先を無効にして描かない
    crs.Cameras().Update(g_fallback_camera, common);
}
}

void CrsBridge::SubmitMeshRenderer(CRS::MeshRendererId &id, const std::shared_ptr<Mesh> &mesh,
                                   const std::vector<AssetPtr<Material>> &materials, const Matrix &world,
                                   const DirectX::BoundingBox &local_bounds, const CRS::BufferHandle bone_buffer)
{
    const auto gpu_data = mesh ? mesh->GpuData() : nullptr;
    if (gpu_data == nullptr)
    {
        RemoveMeshRenderer(id);
        return;
    }

    CRS::MeshRendererData data;
    data.vertex_buffer = gpu_data->vertices.GetHandle();
    data.index_buffer = gpu_data->indices.GetHandle();
    data.vertex_layout = Vertex::Layout();
    data.bone_buffer = bone_buffer;
    data.world = world;
    data.local_bounds = local_bounds;

    const auto range_count = std::min(materials.size(), gpu_data->parts.size());
    for (size_t i = 0; i < range_count; ++i)
    {
        const auto material = materials[i].CastedLock();
        if (material == nullptr)
            continue;

        const auto [first_index, index_count] = gpu_data->parts[i];
        data.ranges.push_back({first_index, index_count, 0, material->GetRendererMaterial()});
    }

    auto &store = Crs().Meshes();
    if (store.IsValid(id))
        store.Update(id, std::move(data));
    else
        id = store.Register(std::move(data));
}

void CrsBridge::RemoveMeshRenderer(CRS::MeshRendererId &id)
{
    if (id.IsValid())
        Crs().Meshes().Unregister(id);

    id = {};
}

void CrsBridge::SubmitCamera(CRS::CameraId &id, const CameraComponent &camera)
{
    auto &crs = Crs();
    if (crs.Cameras().GetCommon(id) == nullptr)
        id = crs.CreateCamera();

    auto property = camera.property;
    CRS::CameraCommon common;

    const bool is_main = CameraComponent::Main().get() == &camera;
    if (is_main)
    {
        // 画面に描く。アスペクト比はWindowに合わせる
        const auto window_size = WindowSize();
        property.aspect_ratio = window_size.x / window_size.y;
        common.viewport = {0, 0, window_size.x, window_size.y, 0, 1};
        common.color_target = CrsBridge::Device().GetBackBuffer();
        common.depth_target = crs.MainDepth();
        common.draw_overlays = true;
        // 画面は最後に描く（RenderTextureに描いた結果を使えるように）
        common.priority = 100;
        g_main_camera_submitted = true;
    }
    else if (const auto render_texture = camera.TargetTexture())
    {
        common.color_target = render_texture->GpuTexture();
        const auto target_desc = crs.Device().GetTextureDesc(common.color_target);
        property.aspect_ratio = static_cast<float>(target_desc.width) / static_cast<float>(target_desc.height);
        common.viewport = {
            0, 0, static_cast<float>(target_desc.width), static_cast<float>(target_desc.height), 0, 1
        };
        common.depth_target = GetCameraDepth(camera, target_desc.width, target_desc.height);
    }
    // 描画先が無いカメラは、描画先を無効にして描かない

    const auto transform = camera.GameObject()->Transform();
    common.view = camera.ViewMatrix();
    common.projection = property.ProjectionMatrix();
    common.position = transform->Position();
    common.clear_color = {
        property.background_color.R(), property.background_color.G(), property.background_color.B(),
        property.background_color.A()
    };
    crs.Cameras().Update(id, common);
}

void CrsBridge::RemoveCamera(CRS::CameraId &id)
{
    if (id.IsValid())
        Crs().DestroyCamera(id);

    id = {};
}

void CrsBridge::AddUiImage(const Matrix &screen_matrix, const std::shared_ptr<Material> &material)
{
    // Quadの頂点とUV（画像の上がUVの上になる向き）
    const std::array corners = {
        Vector2::Transform(Vector2(-1, 1), screen_matrix), Vector2::Transform(Vector2(1, 1), screen_matrix),
        Vector2::Transform(Vector2(-1, -1), screen_matrix), Vector2::Transform(Vector2(1, -1), screen_matrix)
    };
    const std::array uvs = {Vector2(0, 1), Vector2(1, 1), Vector2(0, 0), Vector2(1, 0)};

    CRS::TextureHandle texture;
    CRS::MaterialId ui_material;
    if (material != nullptr)
    {
        if (const auto main_texture = material->GetTexture("_MainTex").CastedLock())
            texture = main_texture->GpuTexture();

        // UI用のShaderなら、そのShaderとMaterialの値で描く
        const auto shader = material->GetShader().CastedLock();
        if (shader != nullptr && shader->IsUiShader())
            ui_material = material->GetRendererMaterial();
    }

    Crs().Immediate().AddUiQuad(texture, corners, uvs, Color(1, 1, 1, 1), ui_material);
}

void CrsBridge::AddUiText(const std::shared_ptr<FontData> &font, const Vector2 position, const std::string &string,
                          const Color &color)
{
    if (font == nullptr || string.empty())
        return;

    const auto texture = font->GpuTexture();
    if (!texture.IsValid())
        return;

    const auto &texture_data = font->Texture();
    const auto texture_size = Vector2(static_cast<float>(texture_data.width), static_cast<float>(texture_data.height));
    const auto screen_size = WindowSize();
    auto &immediate = Crs().Immediate();

    font->ForEachGlyph(string, [&](const FontData::Glyph &glyph, const Vector2 offset) {
        const auto top_left = position + offset;
        const auto size = Vector2(static_cast<float>(glyph.right - glyph.left),
                                  static_cast<float>(glyph.bottom - glyph.top));

        const std::array corners = {
            PixelToNdc(top_left, screen_size), PixelToNdc(top_left + Vector2(size.x, 0), screen_size),
            PixelToNdc(top_left + Vector2(0, size.y), screen_size), PixelToNdc(top_left + size, screen_size)
        };

        const auto uv_min = Vector2(static_cast<float>(glyph.left), static_cast<float>(glyph.top)) / texture_size;
        const auto uv_max = Vector2(static_cast<float>(glyph.right), static_cast<float>(glyph.bottom)) / texture_size;
        const std::array uvs = {uv_min, Vector2(uv_max.x, uv_min.y), Vector2(uv_min.x, uv_max.y), uv_max};

        immediate.AddUiQuad(texture, corners, uvs, color);
    });
}

void CrsBridge::PrepareFrame()
{
    SubmitGizmoLines();
    SubmitLights();
    SubmitEnvironment();
    SubmitFallbackCamera();
    g_main_camera_submitted = false;
}

void CrsBridge::RenderOverlay()
{
    const auto back_buffer = g_device->GetBackBuffer();
    g_crs->CommandList().Transition(back_buffer, CRS::ResourceState::RenderTarget);

    const auto cmd_list = g_device->NativeCommandList();
    const auto rtv = g_device->GetRtv(back_buffer);
    cmd_list->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

    const auto size = WindowSize();
    const D3D12_VIEWPORT viewport = {0, 0, size.x, size.y, 0, 1};
    const D3D12_RECT scissor = {0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y)};
    cmd_list->RSSetViewports(1, &viewport);
    cmd_list->RSSetScissorRects(1, &scissor);

    g_device->BindDescriptorHeaps(cmd_list);

    on_overlay_render.Invoke();
}

bool CrsBridge::Init(const HWND hwnd, const uint32_t width, const uint32_t height)
{
    auto crs = CRS::CreateDefaultRenderEngine(hwnd, width, height);
    if (crs == nullptr)
    {
        Logger::Error<CrsBridge>("Failed to create CaboRenderingSystem");
        return false;
    }

    g_crs = crs.release();
    g_device = static_cast<CRS::D3D12RenderDevice *>(&g_crs->Device());

    Application::on_window_resized.AddListener([] {
        Logger::Log<CrsBridge>("Resizing back buffers");
        g_crs->Resize(Application::WindowWidth(), Application::WindowHeight());
    });

    Logger::Log<CrsBridge>("CaboRenderingSystem initialized");
    return true;
}

void CrsBridge::BeginFrame()
{
    g_crs->BeginFrame();
}

void CrsBridge::Render()
{
    PrepareFrame();

    const auto time = Time::Get();
    g_crs->Render(time->TimeSinceStartUp(), Time::GetDeltaTime());

    RenderOverlay();
}

void CrsBridge::EndFrame()
{
    g_crs->EndFrame();
}

void CrsBridge::WaitIdle()
{
    g_device->WaitIdle();
}

CRS::RenderEngine &CrsBridge::Crs()
{
    return *g_crs;
}

CRS::D3D12RenderDevice &CrsBridge::Device()
{
    return *g_device;
}
}
