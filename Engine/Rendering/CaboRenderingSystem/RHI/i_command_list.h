#pragma once
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"

namespace CRS
{
/// <summary>
/// 描画コマンドを記録するInterfaceです。当面はフレームに1本です。
/// </summary>
class ICommandList
{
public:
    virtual ~ICommandList() = default;

    virtual void Transition(TextureHandle texture, ResourceState state) = 0;
    virtual void Transition(BufferHandle buffer, ResourceState state) = 0;

    virtual void BeginRenderPass(const RenderPassDesc &desc) = 0;
    virtual void EndRenderPass() = 0;

    virtual void SetViewport(const Viewport &viewport) = 0;
    virtual void SetScissor(const ScissorRect &scissor) = 0;

    virtual void SetPipelineState(PipelineStateHandle pipeline_state) = 0;
    virtual void SetBindings(const Bindings &bindings) = 0;

    virtual void SetVertexBuffer(BufferHandle buffer, uint32_t stride, uint64_t offset = 0) = 0;
    // CPUの頂点データをその場でアップロードして使います
    virtual void SetVertexData(const TransientVertices &vertices) = 0;
    // インデックスは32bit固定です
    virtual void SetIndexBuffer(BufferHandle buffer, uint64_t offset = 0) = 0;

    virtual void Draw(uint32_t vertex_count, uint32_t instance_count = 1, uint32_t first_vertex = 0,
                      uint32_t first_instance = 0) = 0;
    virtual void DrawIndexed(uint32_t index_count, uint32_t instance_count = 1, uint32_t first_index = 0,
                             int32_t base_vertex = 0, uint32_t first_instance = 0) = 0;

    virtual void CopyBuffer(BufferHandle dst, uint64_t dst_offset, BufferHandle src, uint64_t src_offset,
                            uint64_t size) = 0;

    virtual void BeginDebugEvent(const char *name) = 0;
    virtual void EndDebugEvent() = 0;
};
}
