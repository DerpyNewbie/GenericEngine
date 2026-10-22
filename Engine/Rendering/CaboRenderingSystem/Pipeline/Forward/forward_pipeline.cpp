#include "Rendering/CaboRenderingSystem/Pipeline/Forward/forward_pipeline.h"

#include <algorithm>

#include "Rendering/CaboRenderingSystem/Core/log.h"
#include "Rendering/CaboRenderingSystem/Pipeline/Forward/forward_passes.h"

namespace CRS
{
namespace
{
constexpr auto kShaderDirectory = L"Engine/Rendering/CaboRenderingSystem/Shaders/";

Shader LoadShader(IRenderDevice &device, const wchar_t *file_name, const wchar_t *entry_point,
                  const ShaderStage stage)
{
    Shader shader(device, {std::wstring(kShaderDirectory) + file_name, entry_point, stage, {}});
    if (!shader.IsValid())
        LogError("Failed to load shader '{}'", std::string(file_name, file_name + wcslen(file_name)));
    return shader;
}
}

ForwardPipeline::ForwardPipeline(IRenderDevice &device, const ForwardRenderingSettings &settings) :
    m_device_(device), m_settings_(settings), m_draw_list_builder_(device),
    m_shadow_system_(device, settings.shadow_map_size, settings.max_shadow_maps)
{
    m_standard_vertex_shader_ = LoadShader(device, L"forward_standard.hlsl", L"VSMain", ShaderStage::Vertex);
    m_standard_pixel_shader_ = LoadShader(device, L"forward_standard.hlsl", L"PSMain", ShaderStage::Pixel);
    m_shadow_vertex_shader_ = LoadShader(device, L"forward_standard.hlsl", L"VSShadow", ShaderStage::Vertex);
    m_skybox_vertex_shader_ = LoadShader(device, L"skybox.hlsl", L"VSMain", ShaderStage::Vertex);
    m_skybox_pixel_shader_ = LoadShader(device, L"skybox.hlsl", L"PSMain", ShaderStage::Pixel);
    m_line_vertex_shader_ = LoadShader(device, L"line.hlsl", L"VSMain", ShaderStage::Vertex);
    m_line_pixel_shader_ = LoadShader(device, L"line.hlsl", L"PSMain", ShaderStage::Pixel);
    m_ui_vertex_shader_ = LoadShader(device, L"ui.hlsl", L"VSMain", ShaderStage::Vertex);
    m_ui_pixel_shader_ = LoadShader(device, L"ui.hlsl", L"PSMain", ShaderStage::Pixel);

    // テクスチャが指定されていないスロット用の1x1の白
    constexpr uint32_t white = 0xFFFFFFFF;
    TextureDesc white_desc;
    white_desc.format = Format::R8G8B8A8_UNorm;
    white_desc.usage = TextureUsage::ShaderResource;
    white_desc.initial_data = &white;
    white_desc.initial_row_pitch = sizeof(white);
    white_desc.debug_name = "CRS WhiteTexture";
    m_white_texture_ = Texture(device, white_desc);

    m_default_sampler_ = Sampler(device, SamplerDesc{Filter::Linear, AddressMode::Wrap, 8});

    // 登録順 = 実行順
    AddPass(std::make_unique<ShadowPass>());
    AddPass(std::make_unique<SkyboxPass>(m_skybox_vertex_shader_.GetHandle(), m_skybox_pixel_shader_.GetHandle()));
    AddPass(std::make_unique<ForwardPass>());
    AddPass(std::make_unique<LinePass>(m_line_vertex_shader_.GetHandle(), m_line_pixel_shader_.GetHandle()));
    AddPass(std::make_unique<UiPass>(m_ui_vertex_shader_.GetHandle(), m_ui_pixel_shader_.GetHandle()));
}

std::unique_ptr<ICameraSettings> ForwardPipeline::CreateCameraSettings() const
{
    return std::make_unique<ForwardCameraSettings>();
}

Material ForwardPipeline::CreateDefaultMaterial() const
{
    Material material;

    MaterialPass forward;
    forward.vertex_shader = m_standard_vertex_shader_.GetHandle();
    forward.pixel_shader = m_standard_pixel_shader_.GetHandle();
    material.SetPass(PassTag::Forward, forward);

    MaterialPass shadow;
    shadow.vertex_shader = m_shadow_vertex_shader_.GetHandle();
    shadow.render_state.cull = CullMode::None;
    material.SetPass(PassTag::Shadow, shadow);

    material.BuildPropertyTable(m_device_);
    material.SetColor("_Color", Color(1, 1, 1, 1));
    return material;
}

void ForwardPipeline::AddPass(std::unique_ptr<IRenderPass> pass)
{
    m_passes_.push_back(std::move(pass));
}

std::vector<RenderView> ForwardPipeline::BuildCameraViews(const FrameSnapshot &frame) const
{
    std::vector<const CameraSnapshot *> cameras;
    for (const auto &camera : frame.cameras)
        cameras.push_back(&camera);

    std::ranges::stable_sort(cameras, [](const CameraSnapshot *a, const CameraSnapshot *b) {
        return a->common.priority < b->common.priority;
    });

    std::vector<RenderView> views;
    views.reserve(cameras.size());
    for (const auto camera : cameras)
    {
        const auto &common = camera->common;
        const auto settings = static_cast<const ForwardCameraSettings *>(camera->settings.get());
        if (!common.color_target.IsValid())
            continue;

        RenderView view;
        view.kind = RenderViewKind::Camera;
        view.view = common.view;
        view.projection = common.projection;
        view.view_projection = common.view * common.projection;
        view.previous_view_projection = common.previous_view_projection;
        view.position = common.position;
        view.viewport = common.viewport;
        view.scissor = {
            static_cast<int32_t>(common.viewport.x), static_cast<int32_t>(common.viewport.y),
            static_cast<int32_t>(common.viewport.x + common.viewport.width),
            static_cast<int32_t>(common.viewport.y + common.viewport.height)
        };
        view.color_target = common.color_target;
        view.depth_target = common.depth_target;
        view.color_format = m_device_.GetTextureDesc(common.color_target).format;
        view.depth_format = common.depth_target.IsValid()
                                ? m_device_.GetTextureDesc(common.depth_target).format
                                : Format::Unknown;
        view.sample_count = settings ? settings->sample_count : 1;
        view.color_load = common.color_load;
        view.depth_load = common.depth_load;
        view.clear_color = common.clear_color;
        view.culling_mask = common.culling_mask;
        view.draw_overlays = common.draw_overlays;
        view.camera_settings = settings;
        views.push_back(view);
    }
    return views;
}

void ForwardPipeline::ClearView(ICommandList &command_list, const RenderView &view)
{
    if (view.color_load != LoadOp::Clear && view.depth_load != LoadOp::Clear)
        return;

    command_list.Transition(view.color_target, ResourceState::RenderTarget);
    if (view.depth_target.IsValid())
        command_list.Transition(view.depth_target, ResourceState::DepthWrite);

    RenderPassDesc pass_desc;
    pass_desc.color_targets[0] = view.color_target;
    pass_desc.color_target_count = 1;
    pass_desc.color_load = view.color_load;
    pass_desc.clear_color = view.clear_color;
    pass_desc.depth_target = view.depth_target;
    pass_desc.depth_load = view.depth_load;
    command_list.BeginRenderPass(pass_desc);
    command_list.EndRenderPass();
}

void ForwardPipeline::RenderSingleView(const FrameSnapshot &frame, ICommandList &command_list,
                                       const MaterialRegistry &materials, const RenderView &view,
                                       const FrameLighting &lighting)
{
    const auto visible = m_culling_stage_.Cull(view, frame.meshes);

    ViewConstants view_constants;
    view_constants.view = view.view;
    view_constants.projection = view.projection;
    view_constants.view_projection = view.view_projection;
    view_constants.previous_view_projection = view.previous_view_projection;
    view_constants.camera_position = Vector4(view.position.x, view.position.y, view.position.z, 1.0f);
    view_constants.time = Vector4(frame.time, frame.delta_time, 0, 0);

    static const DrawList kEmptyDrawList;
    for (const auto &pass : m_passes_)
    {
        if (!pass->AcceptsView(view, frame))
            continue;

        DrawList draw_list;
        if (pass->UsesDrawList())
            draw_list = m_draw_list_builder_.Build(pass->GetTag(), view, visible, frame, materials);

        const PassContext context = {
            command_list, m_device_, view, frame, pass->UsesDrawList() ? draw_list : kEmptyDrawList, materials,
            view_constants, lighting, m_white_texture_.GetHandle(), m_default_sampler_.GetHandle()
        };
        pass->Execute(context);
    }
}

void ForwardPipeline::Render(const FrameSnapshot &frame, ICommandList &command_list,
                             const MaterialRegistry &materials)
{
    m_shadow_system_.BeginFrame();

    for (const auto &camera_view : BuildCameraViews(frame))
    {
        // 影由来のビューも、カメラと同じ経路（カリング → 描画リスト → パス）で処理する
        std::vector<RenderView> shadow_views;
        const auto lighting = m_shadow_system_.Prepare(frame, camera_view, shadow_views);
        for (const auto &shadow_view : shadow_views)
            RenderSingleView(frame, command_list, materials, shadow_view, lighting);

        ClearView(command_list, camera_view);
        RenderSingleView(frame, command_list, materials, camera_view, lighting);

        // テクスチャとして読む描画先（RenderTexture）は、後のカメラが読めるようにしておく
        if (HasFlag(m_device_.GetTextureDesc(camera_view.color_target).usage, TextureUsage::ShaderResource))
            command_list.Transition(camera_view.color_target, ResourceState::ShaderResource);
    }
}
}