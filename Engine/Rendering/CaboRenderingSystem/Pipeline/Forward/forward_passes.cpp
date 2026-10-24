#include "pch.h"
#include "Rendering/CaboRenderingSystem/Pipeline/Forward/forward_passes.h"

namespace CRS
{
namespace
{
// シェーダー側のt (space1)の割り当て
constexpr uint32_t kFrameSlotLights = 0;
constexpr uint32_t kFrameSlotLightViewProjections = 1;
constexpr uint32_t kFrameSlotShadowMaps = 2;
constexpr uint32_t kFrameSlotBones = 3;

PipelineStateDesc MakeSimpleDesc(const RenderView &view, const ShaderHandle vertex_shader,
                                 const ShaderHandle pixel_shader, const bool use_depth)
{
    PipelineStateDesc desc;
    desc.vertex_shader = vertex_shader;
    desc.pixel_shader = pixel_shader;
    desc.render_target_count = 1;
    desc.render_target_formats[0] = view.color_format;
    desc.depth_format = use_depth ? view.depth_format : Format::Unknown;
    desc.sample_count = view.sample_count;
    return desc;
}

/// <summary>
/// Materialのテクスチャを、シェーダーで読める状態にします。
/// RenderTextureなど、別のカメラの描画先になっているテクスチャも読めるようにするためです（静的なテクスチャは何もしません）。
/// 今の描画先そのものは読めないので切り替えません。
/// </summary>
void PrepareMaterialResources(const PassContext &context, const Bindings &bindings)
{
    for (const auto &resource : bindings.resources)
    {
        if (!resource.texture.IsValid() || resource.texture == context.view.color_target ||
            resource.texture == context.view.depth_target)
            continue;

        context.command_list.Transition(resource.texture, ResourceState::ShaderResource);
    }
}

void BeginColorPass(const PassContext &context, const bool use_depth)
{
    auto &command_list = context.command_list;
    const auto &view = context.view;

    command_list.Transition(view.color_target, ResourceState::RenderTarget);
    if (use_depth && view.depth_target.IsValid())
        command_list.Transition(view.depth_target, ResourceState::DepthWrite);

    RenderPassDesc pass_desc;
    pass_desc.color_targets[0] = view.color_target;
    pass_desc.color_target_count = 1;
    if (use_depth)
        pass_desc.depth_target = view.depth_target;
    command_list.BeginRenderPass(pass_desc);
    command_list.SetViewport(view.viewport);
    command_list.SetScissor(view.scissor);
}
}

Bindings MakeMeshBindings(const PassContext &context, const DrawItem &item, ObjectConstants &object_constants)
{
    const auto &mesh = *item.mesh;
    const auto &material = *item.material;

    object_constants.world = mesh.world;
    object_constants.previous_world = mesh.previous_world;
    object_constants.flags = mesh.bone_buffer.IsValid() ? ObjectConstants::kFlagSkinned : 0;

    Bindings bindings;
    bindings.frame_constants[kFrameConstant_View] = ConstantBinding::FromValue(context.view_constants);
    bindings.frame_constants[kFrameConstant_Object] = ConstantBinding::FromValue(object_constants);
    bindings.frame_constants[kFrameConstant_Lighting] = ConstantBinding::FromValue(context.lighting.constants);
    material.BindConstants(bindings);

    material.BindResources(bindings, context.default_texture);

    const auto &samplers = material.Samplers();
    for (uint32_t i = 0; i < kSamplerSlotCount; ++i)
        bindings.samplers[i] = samplers[i].IsValid() ? samplers[i] : context.default_sampler;

    bindings.frame_resources[kFrameSlotLights].buffer = context.lighting.lights;
    bindings.frame_resources[kFrameSlotLightViewProjections].buffer = context.lighting.light_view_projections;
    // 影を描いている最中は、影のテクスチャを読まない
    if (context.view.kind != RenderViewKind::Shadow)
        bindings.frame_resources[kFrameSlotShadowMaps].texture = context.lighting.shadow_maps;
    bindings.frame_resources[kFrameSlotBones].buffer = mesh.bone_buffer;
    PrepareMaterialResources(context, bindings);
    return bindings;
}

// ---- ShadowPass ----

PassTag ShadowPass::GetTag() const
{
    return PassTag::Shadow;
}

bool ShadowPass::AcceptsView(const RenderView &view, const FrameSnapshot &) const
{
    return view.kind == RenderViewKind::Shadow && view.depth_target.IsValid();
}

void ShadowPass::Execute(const PassContext &context)
{
    auto &command_list = context.command_list;
    const auto &view = context.view;

    command_list.BeginDebugEvent("CRS Shadow");
    command_list.Transition(view.depth_target, ResourceState::DepthWrite);

    RenderPassDesc pass_desc;
    pass_desc.depth_target = view.depth_target;
    pass_desc.depth_slice = view.depth_slice;
    pass_desc.depth_load = LoadOp::Clear;
    command_list.BeginRenderPass(pass_desc);
    command_list.SetViewport(view.viewport);
    command_list.SetScissor(view.scissor);

    for (const auto &item : context.draw_list)
    {
        ObjectConstants object_constants;
        const auto bindings = MakeMeshBindings(context, item, object_constants);
        command_list.SetPipelineState(item.pipeline_state);
        command_list.SetBindings(bindings);
        command_list.SetVertexBuffer(item.mesh->vertex_buffer, item.mesh->vertex_layout->stride);
        command_list.SetIndexBuffer(item.mesh->index_buffer);
        command_list.DrawIndexed(item.range->index_count, 1, item.range->first_index, item.range->base_vertex);
    }

    command_list.EndRenderPass();
    command_list.EndDebugEvent();
}

// ---- SkyboxPass ----

SkyboxPass::SkyboxPass(const ShaderHandle vertex_shader, const ShaderHandle pixel_shader) :
    m_vertex_shader_(vertex_shader), m_pixel_shader_(pixel_shader)
{}

PassTag SkyboxPass::GetTag() const
{
    return PassTag::Forward;
}

bool SkyboxPass::UsesDrawList() const
{
    return false;
}

bool SkyboxPass::AcceptsView(const RenderView &view, const FrameSnapshot &frame) const
{
    return view.kind == RenderViewKind::Camera && view.color_target.IsValid() && frame.environment.skybox.IsValid();
}

void SkyboxPass::Execute(const PassContext &context)
{
    // 36頂点の立方体（内側から見る）
    static constexpr Vector3 kCube[36] = {
        {-1, 1, -1}, {1, 1, -1}, {1, -1, -1}, {-1, 1, -1}, {1, -1, -1}, {-1, -1, -1},
        {1, 1, 1}, {-1, 1, 1}, {-1, -1, 1}, {1, 1, 1}, {-1, -1, 1}, {1, -1, 1},
        {1, 1, -1}, {1, 1, 1}, {1, -1, 1}, {1, 1, -1}, {1, -1, 1}, {1, -1, -1},
        {-1, 1, 1}, {-1, 1, -1}, {-1, -1, -1}, {-1, 1, 1}, {-1, -1, -1}, {-1, -1, 1},
        {-1, 1, 1}, {1, 1, 1}, {1, 1, -1}, {-1, 1, 1}, {1, 1, -1}, {-1, 1, -1},
        {-1, -1, -1}, {1, -1, -1}, {1, -1, 1}, {-1, -1, -1}, {1, -1, 1}, {-1, -1, 1},
    };

    auto &command_list = context.command_list;
    const auto &view = context.view;

    auto desc = MakeSimpleDesc(view, m_vertex_shader_, m_pixel_shader_, true);
    desc.vertex_layout = {{{"POSITION", 0, Format::R32G32B32_Float, 0}}, sizeof(Vector3)};
    desc.render_state.cull = CullMode::None;
    desc.render_state.depth = {true, false, CompareOp::LessEqual};
    const auto pipeline_state = context.device.CreatePipelineState(desc);
    if (!pipeline_state.IsValid())
        return;

    // カメラの位置に関係なく見えるよう、Viewの平行移動を除く
    auto view_constants = context.view_constants;
    auto rotation_only = view.view;
    rotation_only._41 = rotation_only._42 = rotation_only._43 = 0.0f;
    view_constants.view_projection = rotation_only * view.projection;

    command_list.BeginDebugEvent("CRS Skybox");
    BeginColorPass(context, true);

    Bindings bindings;
    bindings.frame_constants[kFrameConstant_View] = ConstantBinding::FromValue(view_constants);
    bindings.resources[0].texture = context.frame.environment.skybox;
    bindings.samplers[0] = context.default_sampler;

    command_list.SetPipelineState(pipeline_state);
    command_list.SetBindings(bindings);
    command_list.SetVertexData({kCube, sizeof(kCube), sizeof(Vector3)});
    command_list.Draw(36);

    command_list.EndRenderPass();
    command_list.EndDebugEvent();
}

// ---- ForwardPass ----

PassTag ForwardPass::GetTag() const
{
    return PassTag::Forward;
}

bool ForwardPass::AcceptsView(const RenderView &view, const FrameSnapshot &) const
{
    return view.kind == RenderViewKind::Camera && view.color_target.IsValid();
}

void ForwardPass::Execute(const PassContext &context)
{
    auto &command_list = context.command_list;

    command_list.BeginDebugEvent("CRS Forward");
    if (context.lighting.shadow_maps.IsValid())
        command_list.Transition(context.lighting.shadow_maps, ResourceState::ShaderResource);

    BeginColorPass(context, true);

    for (const auto &item : context.draw_list)
    {
        ObjectConstants object_constants;
        const auto bindings = MakeMeshBindings(context, item, object_constants);
        command_list.SetPipelineState(item.pipeline_state);
        command_list.SetBindings(bindings);
        command_list.SetVertexBuffer(item.mesh->vertex_buffer, item.mesh->vertex_layout->stride);
        command_list.SetIndexBuffer(item.mesh->index_buffer);
        command_list.DrawIndexed(item.range->index_count, 1, item.range->first_index, item.range->base_vertex);
    }

    command_list.EndRenderPass();
    command_list.EndDebugEvent();
}

// ---- LinePass ----

LinePass::LinePass(const ShaderHandle vertex_shader, const ShaderHandle pixel_shader) :
    m_vertex_shader_(vertex_shader), m_pixel_shader_(pixel_shader)
{}

PassTag LinePass::GetTag() const
{
    return PassTag::Forward;
}

bool LinePass::UsesDrawList() const
{
    return false;
}

bool LinePass::AcceptsView(const RenderView &view, const FrameSnapshot &frame) const
{
    return view.kind == RenderViewKind::Camera && view.draw_overlays && frame.immediate != nullptr &&
           !frame.immediate->Lines().empty();
}

void LinePass::Execute(const PassContext &context)
{
    auto &command_list = context.command_list;
    const auto &view = context.view;
    const auto &lines = context.frame.immediate->Lines();

    auto desc = MakeSimpleDesc(view, m_vertex_shader_, m_pixel_shader_, true);
    desc.vertex_layout = {
        {
            {"POSITION", 0, Format::R32G32B32_Float, offsetof(LineVertex, position)},
            {"COLOR", 0, Format::R32G32B32A32_Float, offsetof(LineVertex, color)},
        },
        sizeof(LineVertex)
    };
    desc.topology = PrimitiveTopology::LineList;
    desc.render_state.blend = BlendMode::AlphaBlend;
    desc.render_state.cull = CullMode::None;
    desc.render_state.depth = {true, false, CompareOp::LessEqual};
    const auto pipeline_state = context.device.CreatePipelineState(desc);
    if (!pipeline_state.IsValid())
        return;

    command_list.BeginDebugEvent("CRS Lines");
    BeginColorPass(context, true);

    Bindings bindings;
    bindings.frame_constants[kFrameConstant_View] = ConstantBinding::FromValue(context.view_constants);

    command_list.SetPipelineState(pipeline_state);
    command_list.SetBindings(bindings);
    command_list.SetVertexData({
        lines.data(), static_cast<uint32_t>(lines.size() * sizeof(LineVertex)), sizeof(LineVertex)
    });
    command_list.Draw(static_cast<uint32_t>(lines.size()));

    command_list.EndRenderPass();
    command_list.EndDebugEvent();
}

// ---- UiPass ----

UiPass::UiPass(const ShaderHandle vertex_shader, const ShaderHandle pixel_shader) :
    m_vertex_shader_(vertex_shader), m_pixel_shader_(pixel_shader)
{}

PassTag UiPass::GetTag() const
{
    return PassTag::Forward;
}

bool UiPass::UsesDrawList() const
{
    return false;
}

bool UiPass::AcceptsView(const RenderView &view, const FrameSnapshot &frame) const
{
    return view.kind == RenderViewKind::Camera && view.draw_overlays && frame.immediate != nullptr &&
           !frame.immediate->UiBatches().empty();
}

void UiPass::Execute(const PassContext &context)
{
    auto &command_list = context.command_list;
    const auto &view = context.view;
    const auto &vertices = context.frame.immediate->UiVertices();

    const auto make_desc = [&](const ShaderHandle vertex_shader, const ShaderHandle pixel_shader,
                               const BlendMode blend) {
        auto desc = MakeSimpleDesc(view, vertex_shader, pixel_shader, false);
        desc.vertex_layout = {
            {
                {"POSITION", 0, Format::R32G32_Float, offsetof(UiVertex, position)},
                {"TEXCOORD", 0, Format::R32G32_Float, offsetof(UiVertex, uv)},
                {"COLOR", 0, Format::R32G32B32A32_Float, offsetof(UiVertex, color)},
            },
            sizeof(UiVertex)
        };
        // UIは基本的に半透明で重ねる
        desc.render_state.blend = blend == BlendMode::Opaque ? BlendMode::AlphaBlend : blend;
        desc.render_state.cull = CullMode::None;
        desc.render_state.depth = {false, false, CompareOp::Always};
        return desc;
    };

    const auto default_pipeline_state = context.device.CreatePipelineState(
        make_desc(m_vertex_shader_, m_pixel_shader_, BlendMode::AlphaBlend));
    if (!default_pipeline_state.IsValid())
        return;

    command_list.BeginDebugEvent("CRS UI");
    BeginColorPass(context, false);

    command_list.SetVertexData({
        vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(UiVertex)), sizeof(UiVertex)
    });

    for (const auto &batch : context.frame.immediate->UiBatches())
    {
        Bindings bindings;
        bindings.frame_constants[kFrameConstant_View] = ConstantBinding::FromValue(context.view_constants);
        for (uint32_t i = 0; i < kSamplerSlotCount; ++i)
            bindings.samplers[i] = context.default_sampler;
        for (uint32_t i = 0; i < kResourceSlotCount; ++i)
            bindings.resources[i].texture = context.default_texture;

        auto pipeline_state = default_pipeline_state;
        const auto material = batch.material.IsValid() ? context.materials.Get(batch.material) : nullptr;
        const auto pass = material ? material->GetPass(PassTag::Forward) : nullptr;
        if (pass != nullptr)
        {
            // UI用のShaderのMaterial: そのShaderと、Materialの定数・テクスチャで描く
            pipeline_state = context.device.CreatePipelineState(
                make_desc(pass->vertex_shader, pass->pixel_shader, pass->render_state.blend));
            if (!pipeline_state.IsValid())
                continue;

            material->BindConstants(bindings);
            material->BindResources(bindings, context.default_texture);
        }

        // _MainTex (t0) がMaterialで指定されていなければ、四角形のテクスチャを使う
        if (batch.texture.IsValid() && (material == nullptr || !material->IsResourceSet(0)))
            bindings.resources[0].texture = batch.texture;

        PrepareMaterialResources(context, bindings);
        command_list.SetPipelineState(pipeline_state);
        command_list.SetBindings(bindings);
        command_list.Draw(batch.vertex_count, 1, batch.first_vertex);
    }

    command_list.EndRenderPass();
    command_list.EndDebugEvent();
}
}
