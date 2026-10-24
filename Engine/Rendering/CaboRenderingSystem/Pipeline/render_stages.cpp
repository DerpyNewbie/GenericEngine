#include "pch.h"
#include "Rendering/CaboRenderingSystem/Pipeline/render_stages.h"

#include <cstring>
#include <functional>

namespace CRS
{
std::vector<uint32_t> CullingStage::Cull(const RenderView &view, const std::vector<MeshRendererData> &meshes) const
{
    DirectX::BoundingFrustum frustum;
    DirectX::BoundingFrustum::CreateFromMatrix(frustum, view.projection, true);
    frustum.Transform(frustum, view.view.Invert());

    std::vector<uint32_t> visible;
    visible.reserve(meshes.size());
    for (uint32_t i = 0; i < meshes.size(); ++i)
    {
        const auto &mesh = meshes[i];
        if ((mesh.layer_mask & view.culling_mask) == 0)
            continue;

        if (view.depth_only && !mesh.cast_shadow)
            continue;

        if (!view.frustum_cull)
        {
            visible.push_back(i);
            continue;
        }

        BoundingBox world_bounds;
        mesh.local_bounds.Transform(world_bounds, mesh.world);
        if (frustum.Intersects(world_bounds))
            visible.push_back(i);
    }
    return visible;
}

size_t DrawListBuilder::PipelineKeyHash::operator()(const PipelineKey &key) const
{
    size_t seed = 0;
    const auto combine = [&](const size_t value) {
        seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    };
    combine(key.material.index);
    combine(key.material.generation);
    combine(key.material_version);
    combine(static_cast<size_t>(key.pass));
    combine(std::hash<const void *>{}(key.vertex_layout));
    combine(static_cast<size_t>(key.color_format));
    combine(static_cast<size_t>(key.depth_format));
    combine(key.sample_count);
    return seed;
}

DrawListBuilder::DrawListBuilder(IRenderDevice &device) : m_device_(device)
{}

PipelineStateHandle DrawListBuilder::ResolvePipelineState(const MaterialId material_id, const Material &material,
                                                          const MaterialPass &pass, const PassTag tag,
                                                          const RenderView &view, const MeshRendererData &mesh)
{
    const PipelineKey key = {
        material_id, material.Version(), tag, mesh.vertex_layout.get(), view.color_format, view.depth_format,
        view.sample_count
    };

    if (const auto itr = m_pipeline_cache_.find(key);
        itr != m_pipeline_cache_.end())
        return itr->second;

    PipelineStateDesc desc;
    desc.vertex_shader = pass.vertex_shader;
    desc.pixel_shader = pass.pixel_shader;
    desc.vertex_layout = *mesh.vertex_layout;
    desc.render_state = pass.render_state;
    desc.render_target_count = view.color_format == Format::Unknown ? 0 : 1;
    desc.render_target_formats[0] = view.color_format;
    desc.depth_format = view.depth_format;
    desc.sample_count = view.sample_count;

    // 同じ設定のPSOはデバイス側でもキャッシュされている
    const auto handle = m_device_.CreatePipelineState(desc);
    m_pipeline_cache_.emplace(key, handle);
    return handle;
}

DrawList DrawListBuilder::Build(const PassTag tag, const RenderView &view, const std::vector<uint32_t> &visible,
                                const FrameSnapshot &frame, const MaterialRegistry &materials)
{
    DrawList draw_list;
    draw_list.reserve(visible.size());

    for (const auto mesh_index : visible)
    {
        const auto &mesh = frame.meshes[mesh_index];
        if (!mesh.vertex_buffer.IsValid() || !mesh.index_buffer.IsValid() || mesh.vertex_layout == nullptr)
            continue;

        const float distance = (mesh.world.Translation() - view.position).Length();
        uint32_t distance_bits;
        std::memcpy(&distance_bits, &distance, sizeof(distance_bits));

        for (const auto &range : mesh.ranges)
        {
            const auto material = materials.Get(range.material);
            if (material == nullptr)
                continue;

            const auto pass = material->GetPass(tag);
            if (pass == nullptr)
                continue;

            const auto pipeline_state = ResolvePipelineState(range.material, *material, *pass, tag, view, mesh);
            if (!pipeline_state.IsValid())
                continue;

            // 不透明は前から後ろ（PSOでまとめる）、半透明は後ろから前
            uint64_t sort_key = static_cast<uint64_t>(material->render_queue) << 48;
            if (pass->render_state.blend == BlendMode::Opaque)
            {
                sort_key |= static_cast<uint64_t>(pipeline_state.index & 0xFFFF) << 32;
                sort_key |= distance_bits;
            }
            else
            {
                sort_key |= static_cast<uint64_t>(~distance_bits);
            }

            draw_list.push_back({pipeline_state, &mesh, &range, material, sort_key});
        }
    }

    std::ranges::sort(draw_list, [](const DrawItem &a, const DrawItem &b) {
        return a.sort_key < b.sort_key;
    });
    return draw_list;
}
}
