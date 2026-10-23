#include "pch.h"
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"

#include <functional>

namespace CRS
{
namespace
{
template <typename T>
void HashCombine(size_t &seed, const T &value)
{
    seed ^= std::hash<T>{}(value) + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
}
}

size_t PipelineStateDesc::Hash() const
{
    size_t seed = 0;
    HashCombine(seed, vertex_shader.index);
    HashCombine(seed, vertex_shader.generation);
    HashCombine(seed, pixel_shader.index);
    HashCombine(seed, pixel_shader.generation);
    HashCombine(seed, vertex_layout.stride);
    for (const auto &attribute : vertex_layout.attributes)
    {
        HashCombine(seed, attribute.semantic);
        HashCombine(seed, attribute.semantic_index);
        HashCombine(seed, static_cast<int>(attribute.format));
        HashCombine(seed, attribute.offset);
    }
    HashCombine(seed, static_cast<int>(render_state.blend));
    HashCombine(seed, static_cast<int>(render_state.cull));
    HashCombine(seed, render_state.depth.test_enable);
    HashCombine(seed, render_state.depth.write_enable);
    HashCombine(seed, static_cast<int>(render_state.depth.compare));
    HashCombine(seed, static_cast<int>(topology));
    HashCombine(seed, render_target_count);
    for (const auto format : render_target_formats)
        HashCombine(seed, static_cast<int>(format));
    HashCombine(seed, static_cast<int>(depth_format));
    HashCombine(seed, sample_count);
    return seed;
}
}
